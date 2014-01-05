#ifndef _graph_hpp_
#define _graph_hpp_

#include <math.h>
#include <vector>
#include <memory>

#include "matrix.hpp"
#include "geometry.hpp"
#include "texture.hpp"
#include "program.hpp"
#include "context.hpp"

class Node {
public:
    virtual void Render(rendering_context& ctx) = 0;
};

struct ClippingVolume {
    float left;
    float right;
    float bottom;
    float top;
    float nearp;
    float farp;
};

// cf http://www.codecolony.de/opengl.htm#camera2
struct Camera {
    Camera(const ClippingVolume& clippingVolume);
    virtual void Render(std::shared_ptr<Node> node, rendering_context& ctx, std::shared_ptr<Program> program) = 0;
    void reset();
    void rotateX(float deg);
    void rotateY(float deg);
    void rotateZ(float deg);
    void moveRight(float dist);
    void moveLeft(float dist);
    void moveUp(float dist);
    void moveDown(float dist);
    void moveForward(float dist);
    void moveBackward(float dist);
    Matrix44 positionAndOrient();
    Vector3 positionV;
    Vector3 directionV;
    Vector3 rightV;
    Vector3 upV;
    ClippingVolume clippingVolume;
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const ClippingVolume& clippingVolume);
    virtual void Render(std::shared_ptr<Node> node, rendering_context& ctx, std::shared_ptr<Program> program);
};

class ParallelCamera : public Camera {
public:
    ParallelCamera(const ClippingVolume& clippingVolume);
    virtual void Render(std::shared_ptr<Node> node, rendering_context& ctx, std::shared_ptr<Program> program);
};

class Group : public Node {
public:
    Group();
    void Transformation(const Matrix44& tr);
    void Add(std::shared_ptr<Node> node);
    virtual void Render(rendering_context& ctx);
protected:
    std::vector<std::shared_ptr<Node>> children;
    Matrix44 transformation;
};

template<class T>
class GeometryNode : public Node {
public:
    GeometryNode(std::shared_ptr<Geometry<T>> geom) : geom(geom) {}
    virtual void Render(rendering_context& ctx) {
        ctx.program->Render(*geom, ctx);
    }
private:
    std::shared_ptr<Geometry<T>> geom;
};

#endif