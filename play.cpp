#include <iostream>
#include <SFML/Graphics.hpp>

#include "play.hpp"
#include "timer.hpp"
#include "graph.hpp"
#include "geometry.hpp"
#include "misc.hpp"
#include "texture.hpp"

enum class direction {
    none, up, down, right, left
};

struct game_data {
    game_data(MazeModel& model) : model(model) {}
    MazeModel& model;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Group> hero;
    int pos_x;
    int pos_y;
    float pos_fx;
    float pos_fy;
    direction dir;
};

std::shared_ptr<Camera> create_camera(MazeModel& model, sf::RenderWindow& window) {
    ClippingVolume cv;
    int div = 100;
    cv.right = (float)window.getSize().x / div;
    cv.left = (float)-(int)window.getSize().x / div;
    cv.bottom = (float)-(int)window.getSize().y / div;
    cv.top = (float)window.getSize().y / div;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    return std::make_shared<ParallelCamera>(ParallelCamera(cv));
}

void update_position(game_data& g, rendering_context& ctx) {
    float inc = 0.05f;
    switch (g.dir) {
    case direction::left:
        if (!g.model.is_wall(g.pos_x - 1, g.pos_y)) g.pos_fx -= inc;
        break;
    case direction::right:
        if (!g.model.is_wall(g.pos_x + 1, g.pos_y)) g.pos_fx += inc;
        break;
    case direction::up:
        if (!g.model.is_wall(g.pos_x, g.pos_y + 1)) g.pos_fy += inc;
        break;
    case direction::down:
        if (!g.model.is_wall(g.pos_x, g.pos_y - 1)) g.pos_fy -= inc;
        break;
    default:
        break;
    };
    if (abs(g.pos_fx - round(g.pos_fx)) < inc / 10.0f) { g.pos_x = (int)round(g.pos_fx); g.pos_fx = g.pos_x; }
    if (abs(g.pos_fy - round(g.pos_fy)) < inc / 10.0f) { g.pos_y = (int)round(g.pos_fy); g.pos_fy = g.pos_y; }
    g.camera->positionV = Vector3(g.pos_fx, g.pos_fy, 0);
    g.hero->Transformation(Translation(g.pos_fx, g.pos_fy, 0.0f));
}

void play(MazeModel& model, sf::RenderWindow& window, Color& color) {

    timer timer_absolute;
    timer timer_frame;

    sf::Image heroImage;
    if (!heroImage.loadFromFile("smiley.png")) {
        std::cout << "Failed to load smiley.png" << std::endl;
    }
    heroImage.flipVertically();
    auto heroTexture = std::make_shared<Texture>((GLubyte*)heroImage.getPixelsPtr(), heroImage.getSize().x, heroImage.getSize().y);

    HeroBuilder2D heroBuilder;
    std::shared_ptr<Geometry<float>> hero = heroBuilder.build();
    std::shared_ptr<GeometryNode<float>> heroNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(hero));
    MazeGeometryBuilder2D builder2d(model);
    std::shared_ptr<Geometry<float>> mazeGeom2d = builder2d.build();
    std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom2d));

    game_data game = game_data(model);
    game.pos_x = 0;
    game.pos_y = 1;
    game.pos_fx = 0;
    game.pos_fy = 1;
    game.camera = create_camera(model, window);
    game.camera->moveUp(1.5f);
    game.camera->moveRight(0.5f);

    auto root = std::make_shared<Group>(Group());
    root->Add(mazeNode);
    auto root2 = std::make_shared<Group>(Group());
    root2->Add(heroNode);
    game.hero = root2;

    rendering_context ctx;
    ctx.color = color;
    ctx.texture = heroTexture;
    std::shared_ptr<MonochromeProgram> monochromeProgram = MonochromeProgram::Create();
    std::shared_ptr<TextureProgram> textureProgram = TextureProgram::Create();

    while (true)
    {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_time_seconds = timer_frame.elapsed();
        timer_frame.reset();
        CheckForOpenGLErrors();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return;
            }
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    return;
                    break;
                case sf::Keyboard::Left:
                    game.dir = direction::left;
                    break;
                case sf::Keyboard::Right:
                    game.dir = direction::right;
                    break;
                case sf::Keyboard::Up:
                    game.dir = direction::up;
                    break;
                case sf::Keyboard::Down:
                    game.dir = direction::down;
                    break;
                }
            }
        }
        update_position(game, ctx);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        game.camera->Render(root, ctx, monochromeProgram);
        glDisable(GL_DEPTH_TEST);
        game.camera->Render(root2, ctx, textureProgram);
        window.display();
    }
}


