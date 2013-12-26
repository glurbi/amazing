#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>

#include "misc.hpp"
#include "timer.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "program.hpp"
#include "graph.hpp"

enum class menu_choice {
    undefined,
    select_maze,
    next_maze,
    previous_maze,
    exit
};

menu_choice show_maze(MazeModel& model, sf::RenderWindow& window) {

    timer timer_absolute;
    timer timer_frame;

    MazeGeometryBuilder3D builder3d(model);
    std::shared_ptr<Geometry<float>> mazeGeom3d = builder3d.build();
    std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom3d));

    float mf = 1.0f; // margin factor, i.e. how much blank space around the maze

    ClippingVolume cv;
    cv.right = model.get_width() * mf;
    cv.left = -model.get_width() * mf;
    cv.bottom = -model.get_height() * mf;
    cv.top = model.get_height() * mf;
    cv.nearp = (float) (model.get_width() + model.get_height());
    cv.farp = (float) -(model.get_width() + model.get_height());
    ParallelCamera camera(cv);

    auto root = std::make_shared<Group>(Group());
    auto gr1 = std::make_shared<Group>(Group());
    auto gr2 = std::make_shared<Group>(Group());
    gr2->Transformation(Translation(-model.get_width() / 2.0f + 0.5f, -model.get_height() / 2.0f + 0.5f, 0.0f));
    gr2->Add(mazeNode);
    gr1->Add(gr2);
    root->Add(gr1);

    RenderingContext ctx;
    ctx.dir = Vector3(0, 0, -1.0f);
    ctx.color = Color(0.0f, 1.0f, 0.0f);

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
                glViewport(0, 0, event.size.width, event.size.height);
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
                    choice = menu_choice::select_maze;
                    break;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        root->Transformation(Rotation((float) sin(ctx.elapsed_time_seconds / 2) * 180, 1.0f, 0.0f, 0.0f));
        gr1->Transformation(Rotation((float) sin(ctx.elapsed_time_seconds) * 180, 0.0f, 1.0f, 0.0f));
        camera.Render(root, ctx, flatShadingProgram);

        window.pushGLStates();
        float ratio = 20.0f;
        float radius = window.getSize().x / 20.0f;
        sf::CircleShape leftCircle = sf::CircleShape(radius);
        leftCircle.setOutlineThickness(2.0f);
        leftCircle.setPosition(sf::Vector2f(leftCircle.getOutlineThickness(), window.getSize().y / 2.0f - radius));
        leftCircle.setFillColor(sf::Color(255, 255, 255, 140));
        window.draw(leftCircle);
        sf::ConvexShape left_arrow = sf::ConvexShape(3);
        left_arrow.setFillColor(sf::Color::White);
        left_arrow.setPoint(0, sf::Vector2f(radius*.4f, 0));
        left_arrow.setPoint(1, sf::Vector2f(radius*.4f, radius*.8f));
        left_arrow.setPoint(2, sf::Vector2f(0, radius*.4f));
        window.draw(left_arrow);
        window.popGLStates();

        window.display();
    }

    return choice;
}

void menu(sf::RenderWindow& window) {
    const int mazeWidth = 41;
    const int mazeHeight = 41;
    const int width = mazeWidth * 20;
    const int height = mazeHeight * 20;
    MazeModel model(mazeWidth, mazeHeight);
	model.create();

    MazeGeometryBuilder3D builder3d(model);
    std::shared_ptr<Geometry<float>> mazeGeom3d = builder3d.build();
    std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom3d));

    show_maze(model, window);
}

int main() {
    srand((unsigned int)time(0));
    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.depthBits = 16;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Amazing!", sf::Style::Default, settings);
    //sf::RenderWindow window(sf::VideoMode::getFullscreenModes()[0], "Amazing!", sf::Style::Fullscreen, settings);
    window.setMouseCursorVisible(false);
    glewInit();
    glViewport(0, 0, window.getSize().x, window.getSize().y);
    menu(window);
}