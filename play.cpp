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

struct actor_data {
    std::shared_ptr<group> hero_group;
    int pos_x;
    int pos_y;
    float pos_fx;
    float pos_fy;
    direction dir;
    direction next_direction;
    float inc;
};

struct game_data {
    game_data(maze_model& model) : model(model) {}
    maze_model& model;
    std::shared_ptr<camera> camera;
    std::shared_ptr<actor_data> hero_data;
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

void update_position(actor_data& ad, maze_model& model, rendering_context& ctx) {
    switch (ad.dir) {
    case direction::left:
        if (!model.is_wall(ad.pos_x - 1, ad.pos_y)) ad.pos_fx -= ad.inc;
        break;
    case direction::right:
        if (!model.is_wall(ad.pos_x + 1, ad.pos_y)) { ad.pos_fx += ad.inc;  }
        break;
    case direction::up:
        if (!model.is_wall(ad.pos_x, ad.pos_y + 1)) ad.pos_fy += ad.inc;
        break;
    case direction::down:
        if (!model.is_wall(ad.pos_x, ad.pos_y - 1)) ad.pos_fy -= ad.inc;
        break;
    default:
        break;
    };
    if (is_int(ad.pos_fx, ad.inc/10.0f) && (is_int(ad.pos_fy, ad.inc/10.0f))) {
        ad.pos_x = (int)round(ad.pos_fx);
        ad.pos_fx = (float)ad.pos_x;
        ad.pos_y = (int)round(ad.pos_fy);
        ad.pos_fy = (float)ad.pos_y;
        if (ad.next_direction != direction::none) {
            ad.dir = ad.next_direction;
            ad.next_direction = direction::none;
        }
    }
    ad.hero_group->transformation(translation(ad.pos_fx, ad.pos_fy, 0.0f));
}

std::shared_ptr<game_data> make_game_data(maze_model& model, sf::RenderWindow& window) {
    auto game = std::make_shared<game_data>(game_data(model));
    std::shared_ptr<actor_data> hero_data = std::make_shared<actor_data>(actor_data());
    hero_data->pos_x = 0;
    hero_data->pos_y = 1;
    hero_data->pos_fx = 0;
    hero_data->pos_fy = 1;
    game->camera = create_camera(model, window);
    game->camera->move_up(1.5f);
    game->camera->move_right(0.5f);
    hero_data->dir = direction::none;
    hero_data->next_direction = direction::none;
    hero_data->inc = 0.1f;
    game->hero_data = hero_data;
    hero_builder_2d hero_builder;
    auto hero = hero_builder.build();
    auto hero_node = std::make_shared<geometry_node<float>>(geometry_node<float>(hero));
    auto hero_group = std::make_shared<group>(group());
    hero_group->add(hero_node);
    game->hero_data->hero_group = hero_group;
    return game;
}

std::shared_ptr<rendering_context> make_rendering_context(color& c) {
    std::shared_ptr<rendering_context> ctx = std::make_shared<rendering_context>();
    sf::Image heroImage;
    if (!heroImage.loadFromFile("smiley.png")) {
        std::cout << "Failed to load smiley.png" << std::endl;
    }
    heroImage.flipVertically();
    auto heroTexture = std::make_shared<texture>((GLubyte*)heroImage.getPixelsPtr(), heroImage.getSize().x, heroImage.getSize().y);
    ctx->color = c;
    ctx->texture = heroTexture;
    ctx->frame_count = 0;
    return ctx;
}

std::shared_ptr<group> make_maze_group(maze_model& model) {
    auto maze_group = std::make_shared<group>(group());
    maze_geometry_builder_2d builder2d(model);
    auto maze_geom_2d = builder2d.build();
    auto maze_node = std::make_shared<geometry_node<float>>(geometry_node<float>(maze_geom_2d));
    maze_group->add(maze_node);
    return maze_group;
}

void play(maze_model& model, sf::RenderWindow& window, color color, sf::Font& font, sf::Text& text) {

    timer timer_absolute;
    timer timer_frame;

    std::shared_ptr<monochrome_program> monochromeProgram = monochrome_program::create();
    std::shared_ptr<texture_program> textureProgram = texture_program::create();

    auto game = make_game_data(model, window);
    auto maze_group = make_maze_group(model);
    auto ctx = make_rendering_context(color);

    while (true)
    {
        ctx->elapsed_time_seconds = timer_absolute.elapsed();
        ctx->last_frame_times_seconds[ctx->frame_count%100] = timer_frame.elapsed();
        double avg = std::accumulate(ctx->last_frame_times_seconds, ctx->last_frame_times_seconds + 100, 0.0) / 100.0;
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
                game->camera = create_camera(model, window);
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
                    game->hero_data->next_direction = direction::left;
                    break;
                case sf::Keyboard::Right:
                    game->hero_data->next_direction = direction::right;
                    break;
                case sf::Keyboard::Up:
                    game->hero_data->next_direction = direction::up;
                    break;
                case sf::Keyboard::Down:
                    game->hero_data->next_direction = direction::down;
                    break;
                }
            }
        }
        update_position(*game->hero_data, model, *ctx);
        game->camera->position_v = vector3(game->hero_data->pos_fx, game->hero_data->pos_fy, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        game->camera->render(maze_group, *ctx, monochromeProgram);
        glDisable(GL_DEPTH_TEST);
        game->camera->render(game->hero_data->hero_group, *ctx, textureProgram);
        window.display();
        ctx->frame_count++;

        if (game->hero_data->pos_x == model.get_width() - 1 && game->hero_data->pos_y == model.get_height() - 2) {
            victory(window, font, text);
            return;
        }
        //std::cout << game.pos_x << " " << game.pos_y << std::endl;
    }
}


