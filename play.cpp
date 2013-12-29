#include <SFML/Graphics.hpp>

#include "play.hpp"
#include "timer.hpp"
#include "graph.hpp"
#include "geometry.hpp"
#include "misc.hpp"

void play(MazeModel& model, sf::RenderWindow& window) {

    timer timer_absolute;
    timer timer_frame;

    MazeGeometryBuilder2D builder2d(model);
    std::shared_ptr<Geometry<float>> mazeGeom2d = builder2d.build();
    std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom2d));

    ClippingVolume cv;
    cv.right = (float) window.getSize().x / 20;
    cv.left = (float) -(int)window.getSize().x / 20;
    cv.bottom = (float) -(int)window.getSize().y / 20;
    cv.top = (float) window.getSize().y / 20;
    cv.nearp = 1.0f;
    cv.farp = -1.0f;
    ParallelCamera camera(cv);

    auto root = std::make_shared<Group>(Group());
    root->Transformation(Translation(0.0f, 0.0f, 0.0f));
    root->Add(mazeNode);

    rendering_context ctx;
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
                    break;
                case sf::Keyboard::Right:
                    break;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        camera.Render(root, ctx, monochromeProgram);
        window.display();
    }
}


