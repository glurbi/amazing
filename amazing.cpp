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
    std::shared_ptr<Geometry<float>> mazeGeom3d = builder3d.build();

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
    auto rot1 = std::make_shared<Group>(Group());
    rot1->Transformation(Rotation(-20.0, 0.0f, 1.0f, 0.0f));
    auto rot2 = std::make_shared<Group>(Group());
    rot2->Transformation(Rotation(-20.0f, 1.0f, 0.0f, 0.0f));
    rot2->Add(mazeGeom3d);
    rot1->Add(rot2);
    root->Add(rot1);

    RenderingContext ctx;
    ctx.projection(Ortho(mazeWidth * mf, -mazeWidth * mf, mazeHeight * mf, -mazeHeight * mf,
        (mazeWidth + mazeHeight), -(mazeWidth + mazeHeight)));
    ctx.push(Translation(-mazeWidth / 2.0f + 0.5f, -mazeHeight / 2.0f + 0.5f, 0.0f));
    ctx.push(Rotation(-20.0, 0.0f, 1.0f, 0.0f));
    ctx.push(Rotation(-20.0f, 1.0f, 0.0f, 0.0f));

    Vector3 dir = Vector3(-0.5f, -0.5f, -1.0f);
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
