#ifndef _context_hpp_
#define _context_hpp_

#include <vector>
#include <memory>

#include "matrix.hpp"
#include "texture.hpp"
#include "program.hpp"

class Program;

class rendering_context {
public:
    rendering_context();
    void projection(Matrix44 mat);
    void push(Matrix44 mat);
    void pop();
    Matrix44 mvp();
    Matrix44 mv();
    void reset();
    Vector3 dir;
    Color color;
    double elapsed_time_seconds;
    double last_frame_time_seconds;
    std::shared_ptr<Program> program;
    std::shared_ptr<Texture> texture;
private:
    std::vector<Matrix44> mvpStack;
    std::vector<Matrix44> mvStack;
};


#endif
