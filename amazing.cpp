#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>

#include "misc.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "program.hpp"
#include "graph.hpp"

int main()
{
    std::chrono::steady_clock::time_point clock_begin = std::chrono::steady_clock::now();

    srand((unsigned int)time(0));

    const int mazeWidth = 41;
    const int mazeHeight = 41;

    const int width = mazeWidth * 20;
    const int height = mazeHeight * 20;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;
    settings.depthBits = 16;

    sf::RenderWindow window(sf::VideoMode(width, height), "Amazing!", sf::Style::Default, settings);
	glewInit();
	glViewport(0, 0, width, height);

    MazeModel model(mazeWidth, mazeHeight);
	model.create();

    MazeGeometryBuilder3D builder3d(model);
    std::shared_ptr<Geometry<float>> mazeGeom3d = builder3d.build();
    std::shared_ptr<GeometryNode<float>> mazeNode = std::make_shared<GeometryNode<float>>(GeometryNode<float>(mazeGeom3d));

    float mf = 0.7f; // margin factor, i.e. how much blank space around the maze

    ClippingVolume cv;
    cv.right = mazeWidth * mf;
    cv.left = -mazeWidth * mf;
    cv.bottom = -mazeHeight * mf;
    cv.top = mazeHeight * mf;
    cv.nearp = mazeWidth + mazeHeight;
    cv.farp = -(mazeWidth + mazeHeight);
    ParallelCamera camera(cv);

    auto root = std::make_shared<Group>(Group());
    root->Transformation(Translation(-mazeWidth / 2.0f + 0.5f, -mazeHeight / 2.0f + 0.5f, 0.0f));
    auto rot1 = std::make_shared<Group>(Group());
    rot1->Transformation(Rotation(-20.0, 0.0f, 1.0f, 0.0f));
    auto rot2 = std::make_shared<Group>(Group());
    rot2->Transformation(Rotation(-20.0f, 1.0f, 0.0f, 0.0f));
    rot2->Add(mazeNode);
    rot1->Add(rot2);
    root->Add(rot1);

    RenderingContext ctx;
    ctx.dir = Vector3(-0.5f, -0.5f, -1.0f);
    ctx.color = Color(0.0f, 1.0f, 0.0f);
    
    std::shared_ptr<MonochromeProgram> monochromeProgram = MonochromeProgram::Create();
    std::shared_ptr<FlatShadingProgram> flatShadingProgram = FlatShadingProgram::Create();

	bool running = true;
    while (running)
    {
        std::chrono::steady_clock::time_point clock_now = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration time_span = clock_now - clock_begin;
        ctx.elapsed_seconds = double(time_span.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;
        CheckForOpenGLErrors();
		sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
            if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    running = false;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        camera.rotateZ((float)ctx.elapsed_seconds/1000);
        camera.Render(root, ctx, flatShadingProgram);
		window.display();
    }

    return 0;
}
