#ifndef _geometry_hpp_
#define _geometry_hpp_

#include <vector>
#include <memory>
#include <GL/glew.h>

enum VertexAttribute {
    POSITION,
    TEXCOORD
};

template<int posVecSize>
class Geometry {

public:
	Geometry(GLsizei count_) :
        count(count_), positionsId(0), texCoordsId(0) {}
	
	~Geometry() {
        glDeleteBuffers(1, &positionsId);
        glDeleteBuffers(1, &texCoordsId);
    }

	void SetVertexPositions(GLuint positionsId_) {
        positionsId = positionsId_;
    }

    void SetVertexTexCoords(GLuint texCoordsId) {
        texCoordsId = texCoordsId_;
    }

    void SetVertexPositions(void* data, long size) {
        glGenBuffers(1, &positionsId);
        glBindBuffer(GL_ARRAY_BUFFER, positionsId);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

	void SetVertexTexCoords(void* data, long size) {
        glGenBuffers(1, &texCoordsId);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    GLuint GetPositionsId() const {
        return positionsId;
    }

    GLuint GetTexCoordsId() const {
        return texCoordsId;
    }

    GLsizei GetCount() const {
        return count;
    }

private:
	GLuint positionsId;
	GLuint texCoordsId;
    GLsizei count;

};

typedef Geometry<2> Geometry2D;
typedef Geometry<3> Geometry3D;

template <class T>
class BufferObjectBuilder {

public:

    void* GetData() {
        return &data[0];
    }

    GLsizeiptr GetSize() {
        return data.size() * sizeof(T);
    }

    GLuint Build() {
        GLuint id;
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
        return id;
    }

    BufferObjectBuilder<T>& operator<<(T t) {
        data.push_back(t);
        return *this;
    }

private:

    std::vector<T> data;

};

#endif