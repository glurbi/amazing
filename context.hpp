#ifndef _context_hpp_
#define _context_hpp_

#include <vector>
#include <memory>

#include "matrix.hpp"
#include "texture.hpp"

class Program;

struct RenderingContext {
    double elapsed_time_seconds;
    double last_frame_time_seconds;
    std::vector<Matrix44> mvpStack;
    std::vector<Matrix44> mvStack;
    Vector3 dir;
    Color color;
    std::shared_ptr<Program> program;
    Texture* texture;
    RenderingContext();
    void projection(Matrix44& mat);
    void push(Matrix44& mat);
    void pop();
    Matrix44 mvp();
    Matrix44 mv();
    void reset();
};


#endif