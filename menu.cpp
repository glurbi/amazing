#include <iostream>
#include <memory>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <cstdlib>

#include "misc.hpp"
#include "timer.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "program.hpp"
#include "graph.hpp"
#include "menu.hpp"
#include "play.hpp"

void draw_left_arrow(sf::RenderWindow& window, sf::Color& color) {
    window.pushGLStates();
    float ratio = 20.0f;
    float radius = window.getSize().x / ratio;
    sf::CircleShape left_circle = sf::CircleShape(radius);
    left_circle.setOutlineThickness(3.0f);
    left_circle.setOutlineColor(color);
    left_circle.setPosition(sf::Vector2f(radius / 2.0f + left_circle.getOutlineThickness(), window.getSize().y / 2.0f - radius));
    left_circle.setFillColor(sf::Color(255, 255, 255, 140));
    window.draw(left_circle);
    sf::ConvexShape left_arrow = sf::ConvexShape(3);
    left_arrow.setFillColor(color);
    left_arrow.setPosition(sf::Vector2f(radius / 2.0f + left_circle.getOutlineThickness() + radius, window.getSize().y / 2.0f));
    left_arrow.setPoint(0, sf::Vector2f(radius*.7f, -radius*.7f));
    left_arrow.setPoint(1, sf::Vector2f(radius*.7f, radius*.7f));
    left_arrow.setPoint(2, sf::Vector2f(-radius, 0));
    window.draw(left_arrow);
    window.popGLStates();
}

void draw_right_arrow(sf::RenderWindow& window, sf::Color& color) {
    window.pushGLStates();
    float ratio = 20.0f;
    float radius = window.getSize().x / ratio;
    sf::CircleShape right_circle = sf::CircleShape(radius);
    right_circle.setOutlineThickness(3.0f);
    right_circle.setOutlineColor(color);
    right_circle.setPosition(sf::Vector2f(window.getSize().x - radius*2.0f - right_circle.getOutlineThickness() - radius / 2.0f, window.getSize().y / 2.0f - radius));
    right_circle.setFillColor(sf::Color(255, 255, 255, 140));
    window.draw(right_circle);
    sf::ConvexShape right_arrow = sf::ConvexShape(3);
    right_arrow.setFillColor(color);
    right_arrow.setPosition(sf::Vector2f(window.getSize().x - radius - right_circle.getOutlineThickness() - radius / 2.0f, window.getSize().y / 2.0f));
    right_arrow.setPoint(0, sf::Vector2f(-radius*.7f, -radius*.7f));
    right_arrow.setPoint(1, sf::Vector2f(-radius*.7f, radius*.7f));
    right_arrow.setPoint(2, sf::Vector2f(radius, 0));
    window.draw(right_arrow);
    window.popGLStates();
}

std::shared_ptr<Camera> createCamera(MazeModel& model, sf::RenderWindow& window) {
    float aspectRatio = (float)window.getSize().x / window.getSize().y;
    float mf = 0.5f; // margin factor, i.e. how much blank space around the maze
    ClippingVolume cv;
    cv.left = -model.get_width() * mf;
    cv.right = model.get_width() * mf;
    cv.bottom = -model.get_width() / aspectRatio * mf;
    cv.top = model.get_width() / aspectRatio * mf;
    cv.nearp = 1.0f*(model.get_width() + model.get_height());
    cv.farp = 3.0f*(model.get_width() + model.get_height());
    auto camera =  std::make_shared<PerspectiveCamera>(PerspectiveCamera(cv));
    camera->moveBackward(2.0f*(model.get_width()+model.get_height()));
    return camera;
}

menu_choice show_maze(sf::RenderWindow& window, MazeModel& model, bool left_arrow_enabled, bool right_arrow_enabled, const Color& color) {

    timer timer_absolute;
    timer timer_frame;

    MazeGeometryBuilder3D builder3d(model);
    std::shared_ptr<Geometry<float>> mazeGeom3d = builder3d.build();
    std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom3d));

    std::shared_ptr<Camera> camera = createCamera(model, window);

    auto root = std::make_shared<Group>(Group());
    auto gr1 = std::make_shared<Group>(Group());
    auto gr2 = std::make_shared<Group>(Group());
    gr2->Transformation(Translation(-model.get_width() / 2.0f + 0.5f, -model.get_height() / 2.0f + 0.5f, 0.0f));
    gr2->Add(mazeNode);
    gr1->Add(gr2);
    root->Add(gr1);

    rendering_context ctx;
    ctx.dir = Vector3(0, 0, -1.0f);
    ctx.color = color;

    std::shared_ptr<MonochromeProgram> monochromeProgram = MonochromeProgram::Create();
    std::shared_ptr<FlatShadingProgram> flatShadingProgram = FlatShadingProgram::Create();

    menu_choice choice = menu_choice::undefined;
    while (choice == menu_choice::undefined)
    {
        ctx.elapsed_time_seconds = timer_absolute.elapsed();
        ctx.last_frame_time_seconds = timer_frame.elapsed();
        timer_frame.reset();
        CheckForOpenGLErrors();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                choice = menu_choice::exit;
            }
            if (event.type == sf::Event::Resized) {
                camera = createCamera(model, window);
                glViewport(0, 0, event.size.width, event.size.height);
                sf::View view(sf::FloatRect(0, 0, (float) event.size.width, (float) event.size.height));
                window.setView(view);
            }
            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Escape:
                    choice = menu_choice::exit;
                    break;
                case sf::Keyboard::Left:
                    choice = menu_choice::previous_maze;
                    break;
                case sf::Keyboard::Right:
                    choice = menu_choice::next_maze;
                    break;
                case sf::Keyboard::Return:
                    play(model, window, Color(color));
                    break;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        root->Transformation(Rotation((float)sin(ctx.elapsed_time_seconds / 2) * 180, 1.0f, 0.0f, 0.0f));
        gr1->Transformation(Rotation((float)sin(ctx.elapsed_time_seconds) * 180, 0.0f, 1.0f, 0.0f));
        camera->Render(root, ctx, flatShadingProgram);

        sf::Color arrow_colors[] = { sf::Color(128, 128, 128, 255), sf::Color(255, 255, 255, 255) };
        draw_left_arrow(window, arrow_colors[left_arrow_enabled]);
        draw_right_arrow(window, arrow_colors[right_arrow_enabled]);

        window.display();
    }

    return choice;
}

void menu(sf::RenderWindow& window) {
    int index = 0;
    const int len = 10;
    const int sizes[] = { 11, 17, 25, 31, 41, 51, 65, 87, 101, 123, 181 };
    const Color colors[] = {
        Color(0.0f, 1.0f, 0.0f),
        Color(0.0f, 0.0f, 1.0f),
        Color(1.0f, 0.0f, 0.0f),
        Color(0.5f, 0.5f, 0.5f),
        Color(0.0f, 0.7f, 0.9f),
        Color(0.7f, 0.9f, 0.0f),
        Color(0.7f, 0.5f, 0.3f),
        Color(0.5f, 0.3f, 0.7f),
        Color(0.3f, 0.7f, 0.5f),
        Color(1.0f, 0.3f, 0.4f)
    };
    menu_choice choice = menu_choice::undefined;
    while (choice != menu_choice::exit) {
        const int mazeWidth = sizes[index];
        const int mazeHeight = sizes[index];
        MazeModel model(mazeWidth, mazeHeight);
        model.create();
        MazeGeometryBuilder3D builder3d(model);
        std::shared_ptr<Geometry<float>> mazeGeom3d = builder3d.build();
        std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom3d));
        choice = show_maze(window, model, index > 0, index < len - 1, colors[index]);
        switch (choice) {
        case menu_choice::next_maze:
            if (index < len - 1) index++;
            break;
        case menu_choice::previous_maze:
            if (index > 0) index--;
            break;
        case menu_choice::select_maze:
            std::cout << "play with " << index << std::endl;
            break;
        case menu_choice::exit:
            exit(0);
        }
    }
}
