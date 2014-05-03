#include <iostream>
#include <numeric>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <stdlib.h>

#include "play.hpp"
#include "timer.hpp"
#include "graph.hpp"
#include "geometry.hpp"
#include "misc.hpp"
#include "texture.hpp"
#include "victory.hpp"

enum class direction {
    none, up, down, right, left
};

struct game_data {
    game_data(maze_model& model) : model(model) {}
    maze_model& model;
    std::shared_ptr<camera> camera;
    std::shared_ptr<group> hero;
    int pos_x;
    int pos_y;
    float pos_fx;
    float pos_fy;
    direction dir;
    direction next_direction;
    float inc;
};

static std::shared_ptr<camera> create_camera(maze_model& model, sf::RenderWindow& window) {
    clipping_volume cv;
    int div = 100;
    cv.right = (float)window.getSize().x / div;
    cv.left = (float)-(int)window.getSize().x / div;
    cv.bottom = (float)-(int)window.getSize().y / div;
    cv.top = (float)window.getSize().y / div;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    return std::make_shared<parallel_camera>(parallel_camera(cv));
}

bool is_int(float f, float eps) {
    return fabs(f - round(f)) < eps;
}

void update_position(game_data& g, rendering_context& ctx) {
    switch (g.dir) {
    case direction::left:
        if (!g.model.is_wall(g.pos_x - 1, g.pos_y)) g.pos_fx -= g.inc;
        break;
    case direction::right:
        if (!g.model.is_wall(g.pos_x + 1, g.pos_y)) { g.pos_fx += g.inc;  }
        break;
    case direction::up:
        if (!g.model.is_wall(g.pos_x, g.pos_y + 1)) g.pos_fy += g.inc;
        break;
    case direction::down:
        if (!g.model.is_wall(g.pos_x, g.pos_y - 1)) g.pos_fy -= g.inc;
        break;
    default:
        break;
    };
    if (is_int(g.pos_fx, g.inc/10.0f) && (is_int(g.pos_fy, g.inc/10.0f))) {
        g.pos_x = (int)round(g.pos_fx);
        g.pos_fx = (float)g.pos_x;
        g.pos_y = (int)round(g.pos_fy);
        g.pos_fy = (float)g.pos_y;
        if (g.next_direction != direction::none) {
            g.dir = g.next_direction;
            g.next_direction = direction::none;
        }
    }
    g.camera->position_v = vector3(g.pos_fx, g.pos_fy, 0);
    g.hero->transformation(translation(g.pos_fx, g.pos_fy, 0.0f));
}

void play(maze_model& model, sf::RenderWindow& window, color color) {

    timer timer_absolute;
    timer timer_frame;

    sf::Image heroImage;
    if (!heroImage.loadFromFile("smiley.png")) {
        std::cout << "Failed to load smiley.png" << std::endl;
    }
    heroImage.flipVertically();
    auto heroTexture = std::make_shared<texture>((GLubyte*)heroImage.getPixelsPtr(), heroImage.getSize().x, heroImage.getSize().y);

    hero_builder_2d heroBuilder;
    std::shared_ptr<geometry<float>> hero = heroBuilder.build();
    std::shared_ptr<geometry_node<float>> heroNode = std::make_shared<geometry_node<float>>(geometry_node<float>(hero));
    maze_geometry_builder_2d builder2d(model);
    std::shared_ptr<geometry<float>> mazeGeom2d = builder2d.build();
    std::shared_ptr<geometry_node<float>> mazeNode = std::make_shared<geometry_node<float>>(geometry_node<float>(mazeGeom2d));

    game_data game = game_data(model);
    game.pos_x = 0;
    game.pos_y = 1;
    game.pos_fx = 0;
    game.pos_fy = 1;
    game.camera = create_camera(model, window);
    game.camera->move_up(1.5f);
    game.camera->move_right(0.5f);
    game.dir = direction::none;
    game.next_direction = direction::none;
    game.inc = 0.05f;

    auto root = std::make_shared<group>(group());
    root->add(mazeNode);
    auto root2 = std::make_shared<group>(group());
    root2->add(heroNode);
    game.hero = root2;

    rendering_context ctx;
    ctx.color = color;
    ctx.texture = heroTexture;
    std::shared_ptr<monochrome_program> monochromeProgram = monochrome_program::create();
    std::shared_ptr<texture_program> textureProgram = texture_program::create();
    ctx.frame_count = 0;

    while (true)
    {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_times_seconds[ctx.frame_count%100] = timer_frame.elapsed();
        double avg = std::accumulate(ctx.last_frame_times_seconds, ctx.last_frame_times_seconds + 100, 0.0) / 100.0;
        if (avg < 0.01) {
            long usec = (long) ((0.01-avg)*1000000);
            std::this_thread::sleep_for(std::chrono::microseconds(usec));
            //std::cout << "sleeping for " << usec << std::endl;
        }
        timer_frame.reset();
        check_for_opengl_errors();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return;
            }
            if (event.type == sf::Event::Resized) {
                game.camera = create_camera(model, window);
                glViewport(0, 0, event.size.width, event.size.height);
                sf::View view(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height));
                window.setView(view);
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    return;
                    break;
                case sf::Keyboard::Left:
                    game.next_direction = direction::left;
                    break;
                case sf::Keyboard::Right:
                    game.next_direction = direction::right;
                    break;
                case sf::Keyboard::Up:
                    game.next_direction = direction::up;
                    break;
                case sf::Keyboard::Down:
                    game.next_direction = direction::down;
                    break;
                }
            }
        }
        update_position(game, ctx);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        game.camera->render(root, ctx, monochromeProgram);
        glDisable(GL_DEPTH_TEST);
        game.camera->render(root2, ctx, textureProgram);
        window.display();
        ctx.frame_count++;

        if (game.pos_x == model.get_width() - 1 && game.pos_y == model.get_height() - 2) {
            victory(window);
            return;
        }
        //std::cout << game.pos_x << " " << game.pos_y << std::endl;
    }
}


