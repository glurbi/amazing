#include <iostream>
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
    srand(time(0));

    const int mazeWidth = 35;
    const int mazeHeight = 35;

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
    std::shared_ptr<Geometry3D> mazeGeom3d = builder3d.build();

    float mf = 0.7f; // margin factor, i.e. how much blank space around the maze

    ClippingVolume<float> cv;
    cv.right = mazeWidth * mf;
    cv.left = -mazeWidth * mf;
    cv.bottom = -mazeHeight * mf;
    cv.top = mazeHeight * mf;
    cv.nearp = mazeWidth + mazeHeight;
    cv.farp = -(mazeWidth + mazeHeight);
    ParallelCamera<float> camera(cv);

    RenderingContext<float> ctx;
    ctx.projection(Ortho<float>(mazeWidth * mf, -mazeWidth * mf, mazeHeight * mf, -mazeHeight * mf,
        (mazeWidth + mazeHeight), -(mazeWidth + mazeHeight)));
    ctx.push(Translation<float>(-mazeWidth / 2.0f + 0.5f, -mazeHeight / 2.0f + 0.5f, 0.0f));
    ctx.push(Rotation<float>(-20.0, 0.0f, 1.0f, 0.0f));
    ctx.push(Rotation<float>(-20.0f, 1.0f, 0.0f, 0.0f));

    Vector3<float> dir = Vector3<float>(-0.5f, -0.5f, -1.0f);
    Color color = Color(0.0f, 1.0f, 0.0f);
    
    std::shared_ptr<MonochromeProgram> monochromeProgram = MonochromeProgram::Create();
    std::shared_ptr<FlatShadingProgram> flatShadingProgram = FlatShadingProgram::Create();

	bool running = true;
    while (running)
    {
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
        flatShadingProgram->Render(*mazeGeom3d, ctx.mvp(), ctx.mv(), dir, color);
		window.display();
    }

    return 0;
}
