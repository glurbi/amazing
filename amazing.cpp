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
    const int mazeWidth = 25;
    const int mazeHeight = 25;

    const int width = mazeWidth * 30;
    const int height = mazeHeight * 30;

	sf::RenderWindow window(sf::VideoMode(width, height), "Amazing!");
	glewInit();
	glViewport(0, 0, width, height);

    MazeModel model(mazeWidth, mazeHeight);
	model.create();

    MazeGeometryBuilder2D builder2d(model);
    std::shared_ptr<Geometry2D> mazeGeom2d = builder2d.build();

    MazeGeometryBuilder3D builder3d(model);
    std::shared_ptr<Geometry3D> mazeGeom3d = builder3d.build();

    float mf = 0.6f; // margin factor, i.e. how much blank space around the maze
    Matrix44<float> mat = Ortho<float>(mazeWidth * mf, -mazeWidth * mf, mazeHeight * mf, -mazeHeight * mf,
        (mazeWidth + mazeHeight), -(mazeWidth + mazeHeight));

    Matrix44<float> tr = Translation<float>(-mazeWidth/2.0f, -mazeHeight/2.0f, 0.0f);
    Matrix44<float> rot1 = Rotation<float>(-20, 0.0f, 1.0f, 0.0f);
    Matrix44<float> rot2 = Rotation<float>(-10.0f, 1.0f, 0.0f, 0.0f);
    Matrix44<float> rot = Multm(rot1, rot2);
    Matrix44<float> mv = Multm(tr, rot);
    Matrix44<float> mvp = Multm(mat, mv);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        //monochromeProgram->Render(*mazeGeom2d, mat, Color(1.0f, 0.0f, 0.0f));
        flatShadingProgram->Render(*mazeGeom3d, mvp, mv, dir, color);
		window.display();
    }

    return 0;
}
