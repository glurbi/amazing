#include "geometry.hpp"

const int POSITION_ATTRIBUTE_INDEX = 12;
const int TEXCOORD_ATTRIBUTE_INDEX = 7;

Geometry::Geometry() : positionsId(0), texCoordsId(0) {}

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

GLuint Geometry::getPositionsId() const {
    return positionsId;
}

GLuint Geometry::getTexCoordsId() const {
    return texCoordsId;
}
