#include <iostream>
#include <SFML/Graphics.hpp>
#include <GL/glew.h>

#include "misc.hpp"
#include "matrix.hpp"
#include "model.hpp"
#include "geometry.hpp"
#include "program.hpp"

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
    Matrix44<float> mat = Ortho<float>(mazeWidth * mf, -mazeWidth * mf, mazeHeight * mf, -mazeHeight * mf,
        (mazeWidth + mazeHeight), -(mazeWidth + mazeHeight));

    Matrix44<float> tr = Translation<float>(-mazeWidth/2.0f+0.5f, -mazeHeight/2.0f+0.5f, 0.0f);
    Matrix44<float> rot1 = Rotation<float>(-20.0, 0.0f, 1.0f, 0.0f);
    Matrix44<float> rot2 = Rotation<float>(-20.0f, 1.0f, 0.0f, 0.0f);
    Matrix44<float> mv = Multm(tr, tr, rot1, rot2);
    Matrix44<float> mvp = Multm(mat, tr, rot1, rot2);
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
        flatShadingProgram->Render(*mazeGeom3d, mvp, mv, dir, color);
		window.display();
    }

    return 0;
}
