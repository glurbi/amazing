#ifndef _program_hpp_
#define _program_hpp_

#include <string>
#include <map>
#include <memory>
#include <GL/glew.h>

#include "matrix.hpp"
#include "geometry.hpp"
#include "texture.hpp"

template <GLenum type>
class Shader {
public:
    Shader(const std::string& source);
    ~Shader();
    GLuint getId() const;
private:
    GLuint id;
    Shader(const Shader&);
};

class Program {
public:
    Program(const std::string& vertexShaderSource,
            const std::string& fragmentShaderSource,
            const std::map<int, std::string>& attributeIndices);
    ~Program();
protected:
    GLuint id;
private:
    Shader<GL_VERTEX_SHADER> vertexShader;
    Shader<GL_FRAGMENT_SHADER> fragmentShader;
    Program(const Program& that);
};

class MonochromeProgram : public Program {
public:
    void Render(const Geometry<float>& geometry, const Matrix44& mat, const Color& color);
    static std::shared_ptr<MonochromeProgram> Create();
private:
    MonochromeProgram(const std::map<int, std::string>& attributeIndices);
};

class TextureProgram : public Program {
public:
    void Render(const Geometry<float>& geometry, const Texture& texture, const Matrix44& mat);
    static std::shared_ptr<TextureProgram> Create();
private:
    TextureProgram(std::map<int, std::string>& attributeIndices);
};

class FlatShadingProgram : public Program {
public:
    void Render(const Geometry<float>& geometry, const Matrix44& mvp, const Matrix44& mv, const Vector3 dir, const Color& color);
    static std::shared_ptr<FlatShadingProgram> Create();
private:
    FlatShadingProgram(const std::map<int, std::string>& attributeIndices);
};

#endif
