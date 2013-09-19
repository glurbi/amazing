#ifndef _texture_hpp_
#define _texture_hpp_

#include <GL/glew.h>

class Texture {

public:
    Texture(GLubyte* data, GLsizei w, GLsizei h);
    ~Texture();
    GLuint getId() const;

private:
    GLuint id;

};

#endif _texture_hpp_