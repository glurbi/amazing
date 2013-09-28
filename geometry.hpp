#ifndef _geometry_hpp_
#define _geometry_hpp_

#include <vector>
#include <memory>
#include <GL/glew.h>

enum VertexAttribute {
    POSITION,
    TEXCOORD
};

class Geometry {

public:
	Geometry(GLsizei count_);
	Geometry();
	~Geometry();

	void SetVertexPositions(GLuint positionsId_);
	void SetVertexTexCoords(GLuint texCoordsId);

    void SetVertexPositions(void* data, long size);
	void SetVertexTexCoords(void* data, long size);

    GLuint GetPositionsId() const;
    GLuint GetTexCoordsId() const;
    GLsizei GetCount() const;

private:
	GLuint positionsId;
	GLuint texCoordsId;
    GLsizei count;

};

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