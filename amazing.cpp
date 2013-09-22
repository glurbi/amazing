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
    MazeModel model(mazeWidth, mazeHeight);
	model.create();

    const int width = mazeWidth * 20;
    const int height = mazeHeight * 20;
    const float aspectRatio = 1.0f * width / height;

	sf::RenderWindow window(sf::VideoMode(width, height), "Amazing!");
	glewInit();
	glViewport(0, 0, width, height);

    std::vector<float> v;
    for (auto& cell : model.getCells()) {
        if (cell.wall) {
            v.push_back(cell.x-0.5f); v.push_back(cell.y-0.5f); v.push_back(0.0f);
            v.push_back(cell.x+0.5f); v.push_back(cell.y-0.5f); v.push_back(0.0f);
            v.push_back(cell.x+0.5f); v.push_back(cell.y+0.5f); v.push_back(0.0f);
            v.push_back(cell.x-0.5f); v.push_back(cell.y+0.5f); v.push_back(0.0f);
        }
    }
    Geometry mazeGeom(v.size()/3);
    mazeGeom.SetVertexPositions(&v[0], v.size() * sizeof(float));

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
        monochromeProgram->Render(mazeGeom, mat, Color(1.0f, 0.0f, 0.0f));
		window.display();
    }

    return 0;
}
