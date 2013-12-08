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
    const int mazeWidth = 13;
    const int mazeHeight = 13;

    const int width = mazeWidth * 100;
    const int height = mazeHeight * 100;

	sf::RenderWindow window(sf::VideoMode(width, height), "Amazing!");
	glewInit();
	glViewport(0, 0, width, height);

    MazeModel model(mazeWidth, mazeHeight);
	model.create();

    MazeGeometryBuilder2D builder2d(model);
    std::shared_ptr<Geometry2D> mazeGeom2d = builder2d.build();

    MazeGeometryBuilder3D builder3d(model);
    std::shared_ptr<Geometry3D> mazeGeom3d = builder3d.build();

    Matrix44<float> mat = Ortho<float>(mazeWidth * 2, -mazeWidth * 2, mazeHeight * 2, -mazeHeight * 2,
        (mazeWidth + mazeHeight) * 2, -(mazeWidth + mazeHeight) * 2);

    Matrix44<float> tr = Translation<float>(2.0f, 1.0f, 0.0f);
    Matrix44<float> rot = Rotation<float>(-20, 0.0f, 1.0f, 0.0f);
    Matrix44<float> mvp = Multm(mat, rot);
    Vector3<float> dir = Vector3<float>(-1.0f, -1.0f, -1.0f);
    Color color = Color(0.0f, 1.0f, 0.0f);
    
    std::shared_ptr<MonochromeProgram> monochromeProgram = MonochromeProgram::Create();
    std::shared_ptr<FlatShadingProgram> flatShadingProgram = FlatShadingProgram::Create();

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
        //monochromeProgram->Render(*mazeGeom2d, mat, Color(1.0f, 0.0f, 0.0f));
        flatShadingProgram->Render(*mazeGeom3d, mvp, rot, dir, color);
		window.display();
    }

    return 0;
}
