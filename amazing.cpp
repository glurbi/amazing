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
    const int mazeWidth = 39;
    const int mazeHeight = 39;

    const int width = mazeWidth * 20;
    const int height = mazeHeight * 20;

	sf::RenderWindow window(sf::VideoMode(width, height), "Amazing!");
	glewInit();
	glViewport(0, 0, width, height);

    MazeModel model(mazeWidth, mazeHeight);
	model.create();

    MazeGeometryBuilder3D builder(model);
    std::shared_ptr<Geometry> mazeGeom = builder.build();

	Matrix44<float> mat = Ortho<float>(mazeWidth-0.5f, 0-0.5f, mazeHeight-0.5f, 0-0.5f, 1.0f, -1.0f);
    std::shared_ptr<MonochromeProgram> monochromeProgram = MonochromeProgram::Create();

	bool running = true;
    while (running)
    {
        CheckForOpenGLErrors();
		sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
				running = false;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        monochromeProgram->Render(*mazeGeom, mat, Color(1.0f, 0.0f, 0.0f));
		window.display();
    }

    return 0;
}
