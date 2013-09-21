#ifndef _geometry_hpp_
#define _geometry_hpp_

#include <GL/glew.h>

extern const int POSITION_ATTRIBUTE_INDEX;
extern const int TEXCOORD_ATTRIBUTE_INDEX;

class Geometry {

public:
	Geometry(GLsizei count_);
	~Geometry();

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

#endif