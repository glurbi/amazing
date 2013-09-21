#include "geometry.hpp"
#include "misc.hpp"

const int POSITION_ATTRIBUTE_INDEX = 12;
const int TEXCOORD_ATTRIBUTE_INDEX = 7;

Geometry::Geometry(GLsizei count_) : count(count_), positionsId(0), texCoordsId(0) {}

Geometry::~Geometry() {
    glDeleteBuffers(1, &positionsId);
    glDeleteBuffers(1, &texCoordsId);
}

void Geometry::SetVertexPositions(void* data, long size) {
    glGenBuffers(1, &positionsId);
    glBindBuffer(GL_ARRAY_BUFFER, positionsId);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Geometry::SetVertexTexCoords(void* data, long size) {
    glGenBuffers(1, &texCoordsId);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

GLuint Geometry::GetPositionsId() const {
    return positionsId;
}

GLuint Geometry::GetTexCoordsId() const {
    return texCoordsId;
}

GLsizei Geometry::GetCount() const {
    return count;
}
