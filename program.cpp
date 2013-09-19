#include <fstream>
#include <sstream>

#include "program.hpp"

std::string	readTextFile(const std::string& filename) {
    std::ifstream f(filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

template <GLenum type>
Shader<type>::Shader(const std::string& source) {
    id = glCreateShader(type);
    const GLchar* str = source.c_str();
    const GLint length = source.length();
    glShaderSource(id, 1, &str, &length);
    glCompileShader(id);
}

template <GLenum type>
Shader<type>::~Shader() {
    glDeleteShader(id);
}

template <GLenum type>
GLuint Shader<type>::getId() const {
    return id;
}

Program::Program(const std::string& vertexShaderSource,
                 const std::string& fragmentShaderSource,
                 const std::map<int, std::string>& attributeIndices) :
    vertexShader(vertexShaderSource),
    fragmentShader(fragmentShaderSource)
{
    id = glCreateProgram();
    glAttachShader(id, vertexShader.getId());
    glAttachShader(id, fragmentShader.getId());
    for (auto it = attributeIndices.begin(); it != attributeIndices.end(); it++) {
        glBindAttribLocation(id, it->first, it->second.c_str());
    }
    glLinkProgram(id);
}

Program::~Program() {
    glDeleteProgram(id);
}

void MonochromeProgram::Render(const Geometry& geometry, const Matrix44<float>& mat) {
    glUseProgram(id);
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, mat.m);
    GLuint color = glGetUniformLocation(id, "color");
    glUniform4f(color, 1.0f, 1.0f, 0.0f, 0.7f);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_INDEX);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.getPositionsId());
    glVertexAttribPointer(POSITION_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINES, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_INDEX);
}

std::shared_ptr<MonochromeProgram> MonochromeProgram::Create() {
    std::map<int, std::string> monochromeAttributeIndices;
    monochromeAttributeIndices[POSITION_ATTRIBUTE_INDEX] = "vpos";
    return std::shared_ptr<MonochromeProgram>(new MonochromeProgram(monochromeAttributeIndices));
}

MonochromeProgram::MonochromeProgram(const std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("monochrome.vert"), readTextFile("monochrome.frag"), attributeIndices) {}

void TextureProgram::Render(const Geometry& geometry, const Texture& texture, const Matrix44<float>& mat) {
    glUseProgram(id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getId());
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, mat.m);
    GLuint textureUniform = glGetUniformLocation(id, "texture");
    glUniform1i(textureUniform, 0); // we pass the texture unit
    glEnableVertexAttribArray(POSITION_ATTRIBUTE_INDEX);
            glBindBuffer(GL_ARRAY_BUFFER, geometry.getPositionsId());
    glVertexAttribPointer(POSITION_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(TEXCOORD_ATTRIBUTE_INDEX);
            glBindBuffer(GL_ARRAY_BUFFER, geometry.getTexCoordsId());
    glVertexAttribPointer(TEXCOORD_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE_INDEX);
    glDisableVertexAttribArray(TEXCOORD_ATTRIBUTE_INDEX);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<TextureProgram> TextureProgram::Create() {
    std::map<int, std::string> textureAttributeIndices;
    textureAttributeIndices[POSITION_ATTRIBUTE_INDEX] = "pos";
    textureAttributeIndices[TEXCOORD_ATTRIBUTE_INDEX] = "texCoord";
    return std::shared_ptr<TextureProgram>(new TextureProgram(textureAttributeIndices));
}

TextureProgram::TextureProgram(std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("texture.vert"), readTextFile("texture.frag"), attributeIndices) {}

