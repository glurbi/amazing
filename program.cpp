#include <fstream>
#include <sstream>

#include "program.hpp"
#include "misc.hpp"
#include "context.hpp"

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

void MonochromeProgram::Render(const geometry<float>& geometry, rendering_context& ctx) {
    glUseProgram(id);
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, ctx.mvp().m);
    GLuint colorUniform = glGetUniformLocation(id, "color");
    glUniform4f(colorUniform, ctx.color.r(), ctx.color.g(), ctx.color.b(), ctx.color.a());
    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetPositionsId());
    glVertexAttribPointer(vertex_attribute::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.GetCount());
    glDisableVertexAttribArray(vertex_attribute::POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(id);
}

std::shared_ptr<MonochromeProgram> MonochromeProgram::Create() {
    std::map<int, std::string> monochromeAttributeIndices;
    monochromeAttributeIndices[vertex_attribute::POSITION] = "vpos";
    return std::shared_ptr<MonochromeProgram>(new MonochromeProgram(monochromeAttributeIndices));
}

MonochromeProgram::MonochromeProgram(const std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("monochrome.vert"), readTextFile("monochrome.frag"), attributeIndices) {}

void TextureProgram::Render(const geometry<float>& geometry, rendering_context& ctx) {
    glUseProgram(id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx.texture->getId());
    GLuint matrixUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(matrixUniform, 1, false, ctx.mvp().m);
    GLuint textureUniform = glGetUniformLocation(id, "texture");
    glUniform1i(textureUniform, 0); // we pass the texture unit
    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetPositionsId());
    glVertexAttribPointer(vertex_attribute::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertex_attribute::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetTexCoordsId());
    glVertexAttribPointer(vertex_attribute::TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, 4);
    glDisableVertexAttribArray(vertex_attribute::POSITION);
    glDisableVertexAttribArray(vertex_attribute::TEXCOORD);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<TextureProgram> TextureProgram::Create() {
    std::map<int, std::string> textureAttributeIndices;
    textureAttributeIndices[vertex_attribute::POSITION] = "pos";
    textureAttributeIndices[vertex_attribute::TEXCOORD] = "texCoord";
    return std::shared_ptr<TextureProgram>(new TextureProgram(textureAttributeIndices));
}

TextureProgram::TextureProgram(std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("texture.vert"), readTextFile("texture.frag"), attributeIndices) {}

void FlatShadingProgram::Render(const geometry<float>& geometry, rendering_context& ctx) {
    glUseProgram(id);

    GLuint mvpUniform = glGetUniformLocation(id, "mvpMatrix");
    glUniformMatrix4fv(mvpUniform, 1, false, ctx.mvp().m);

    GLuint mvUniform = glGetUniformLocation(id, "mvMatrix");
    glUniformMatrix4fv(mvUniform, 1, false, ctx.mv().m);

    GLuint lightDirUniform = glGetUniformLocation(id, "lightDir");
    glUniform3f(lightDirUniform, ctx.dir.v[0], ctx.dir.v[1], ctx.dir.v[2]);

    GLuint colorUniform = glGetUniformLocation(id, "color");
    glUniform3f(colorUniform, ctx.color.r(), ctx.color.g(), ctx.color.b());

    glEnableVertexAttribArray(vertex_attribute::POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetPositionsId());
    glVertexAttribPointer(vertex_attribute::POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertex_attribute::NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.GetNormalsId());
    glVertexAttribPointer(vertex_attribute::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_QUADS, 0, geometry.GetCount());
    glDisableVertexAttribArray(vertex_attribute::POSITION);
    glDisableVertexAttribArray(vertex_attribute::NORMAL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::shared_ptr<FlatShadingProgram> FlatShadingProgram::Create() {
    std::map<int, std::string> attributeIndices;
    attributeIndices[vertex_attribute::POSITION] = "vPosition";
    attributeIndices[vertex_attribute::NORMAL] = "vNormal";
    return std::shared_ptr<FlatShadingProgram>(new FlatShadingProgram(attributeIndices));
}

FlatShadingProgram::FlatShadingProgram(const std::map<int, std::string>& attributeIndices) :
    Program(readTextFile("flatShading.vert"), readTextFile("flatShading.frag"), attributeIndices) {}
