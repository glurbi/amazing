#include <SFML/Graphics.hpp>

#include "play.hpp"
#include "timer.hpp"
#include "graph.hpp"
#include "geometry.hpp"
#include "misc.hpp"

enum class direction {
    none, up, down, right, left
};

struct game_data {
    game_data(MazeModel& model) : model(model) {}
    MazeModel& model;
    std::shared_ptr<Camera> camera;
    float pos_x;
    float pos_y;
    direction dir;
};

std::shared_ptr<Camera> create_camera(MazeModel& model, sf::RenderWindow& window) {
    ClippingVolume cv;
    cv.right = (float)window.getSize().x / 20;
    cv.left = (float)-(int)window.getSize().x / 20;
    cv.bottom = (float)-(int)window.getSize().y / 20;
    cv.top = (float)window.getSize().y / 20;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    return std::make_shared<ParallelCamera>(ParallelCamera(cv));
}

void update_position(game_data& game, rendering_context& ctx) {
    switch (game.dir) {
    case direction::left:
        if (!game.model.getCell((int)game.pos_x-1, (int)game.pos_y).wall) game.pos_x -= 0.1f;
        break;
    case direction::right:
        if (!game.model.getCell((int)game.pos_x+1, (int)game.pos_y).wall) game.pos_x += 0.1f;
        break;

    };
    game.camera->positionV = Vector3(game.pos_x, game.pos_y, 0);
}

void play(MazeModel& model, sf::RenderWindow& window, Color& color) {

    timer timer_absolute;
    timer timer_frame;

    MazeGeometryBuilder2D builder2d(model);
    std::shared_ptr<Geometry<float>> mazeGeom2d = builder2d.build();
    std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom2d));

    game_data game = game_data(model);
    game.pos_x = 1;
    game.pos_y = 1;
    game.camera = create_camera(model, window);
    game.camera->moveUp(1.0f);
    game.camera->moveRight(1.0f);

    auto root = std::make_shared<Group>(Group());
    root->Add(mazeNode);

    rendering_context ctx;
    ctx.color = color;
    std::shared_ptr<MonochromeProgram> monochromeProgram = MonochromeProgram::Create();

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
        window.display();
    }
}


