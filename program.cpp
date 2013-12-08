#include <fstream>
#include <sstream>

#include "program.hpp"
#include "misc.hpp"

static std::string	readTextFile(const std::string& filename) {
    std::ifstream f(filename);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

static void checkShaderCompileStatus(GLuint shaderId) {
    GLint compileStatus;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        printf("Shader compilation failed...\n");
        char* log = (char*) malloc((1+infoLogLength)*sizeof(char));
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, log);
        log[infoLogLength] = 0;
        printf("%s", log);
    }
}

static void checkProgramLinkStatus(GLuint programId) {
    GLint linkStatus;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
        printf("Program link failed...\n");
        char* log = (char*) malloc((1+infoLogLength)*sizeof(char));
        glGetProgramInfoLog(programId, infoLogLength, NULL, log);
        log[infoLogLength] = 0;
        printf("%s", log);
    }
}

template <GLenum type>
Shader<type>::Shader(const std::string& source) {
    id = glCreateShader(type);
    const GLchar* str = source.c_str();
    const GLint length = source.length();
    glShaderSource(id, 1, &str, &length);
    glCompileShader(id);
    checkShaderCompileStatus(id);
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
    checkProgramLinkStatus(id);
}

Program::~Program() {
    glDeleteProgram(id);
}

void MonochromeProgram::Render(const Geometry2D& geometry, const Matrix44<float>& mat, const Color& color) {
    glUseProgram(id);
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, mat.m);
    GLuint colorUniform = glGetUniformLocation(id, "color");
    glUniform4f(colorUniform, color.r(), color.g(), color.b(), color.a());
    glEnableVertexAttribArray(VertexAttribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetPositionsId());
    glVertexAttribPointer(VertexAttribute::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.GetCount());
    glDisableVertexAttribArray(VertexAttribute::POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(id);
}

std::shared_ptr<MonochromeProgram> MonochromeProgram::Create() {
    std::map<int, std::string> monochromeAttributeIndices;
    monochromeAttributeIndices[VertexAttribute::POSITION] = "vpos";
    return std::shared_ptr<MonochromeProgram>(new MonochromeProgram(monochromeAttributeIndices));
}

MonochromeProgram::MonochromeProgram(const std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("monochrome.vert"), readTextFile("monochrome.frag"), attributeIndices) {}

void TextureProgram::Render(const Geometry3D& geometry, const Texture& texture, const Matrix44<float>& mat) {
    glUseProgram(id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getId());
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, mat.m);
    GLuint textureUniform = glGetUniformLocation(id, "texture");
    glUniform1i(textureUniform, 0); // we pass the texture unit
    glEnableVertexAttribArray(VertexAttribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetPositionsId());
    glVertexAttribPointer(VertexAttribute::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VertexAttribute::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetTexCoordsId());
    glVertexAttribPointer(VertexAttribute::TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(VertexAttribute::POSITION);
    glDisableVertexAttribArray(VertexAttribute::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<TextureProgram> TextureProgram::Create() {
    std::map<int, std::string> textureAttributeIndices;
    textureAttributeIndices[VertexAttribute::POSITION] = "pos";
    textureAttributeIndices[VertexAttribute::TEXCOORD] = "texCoord";
    return std::shared_ptr<TextureProgram>(new TextureProgram(textureAttributeIndices));
}

TextureProgram::TextureProgram(std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("texture.vert"), readTextFile("texture.frag"), attributeIndices) {}

void FlatShadingProgram::Render(const Geometry3D& geometry, const Matrix44<float>& mvp, const Matrix44<float>& mv, const Vector3<float> dir, const Color& color) {
    glUseProgram(id);

    GLuint mvpUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(mvpUniform, 1, false, mvp.m);

    GLuint mvUniform = glGetUniformLocation(id, "mvMatrix");
    glUniformMatrix4fv(mvUniform, 1, false, mv.m);

    GLuint lightDirUniform = glGetUniformLocation(id, "lightDir");
    glUniform3f(lightDirUniform, dir.v[0], dir.v[1], dir.v[2]);

    GLuint colorUniform = glGetUniformLocation(id, "color");
    glUniform3f(colorUniform, color.r(), color.g(), color.b());

    glEnableVertexAttribArray(VertexAttribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetPositionsId());
    glVertexAttribPointer(VertexAttribute::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VertexAttribute::NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetNormalsId());
    glVertexAttribPointer(VertexAttribute::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.GetCount());
    glDisableVertexAttribArray(VertexAttribute::POSITION);
    glDisableVertexAttribArray(VertexAttribute::NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<FlatShadingProgram> FlatShadingProgram::Create() {
    std::map<int, std::string> attributeIndices;
    attributeIndices[VertexAttribute::POSITION] = "vPosition";
    attributeIndices[VertexAttribute::NORMAL] = "vNormal";
    return std::shared_ptr<FlatShadingProgram>(new FlatShadingProgram(attributeIndices));
}

FlatShadingProgram::FlatShadingProgram(const std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("flatShading.vert"), readTextFile("flatShading.frag"), attributeIndices) {}
