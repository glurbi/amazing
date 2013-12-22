#ifndef _graph_hpp_
#define _graph_hpp_

#include <math.h>
#include <vector>
#include <memory>

#include "matrix.hpp"
#include "texture.hpp"

struct RenderingContext {
    std::vector<Matrix44> mvpStack;
    std::vector<Matrix44> mvStack;
    Vector3 dir;
    Color color;
    Texture* texture;
    RenderingContext();
    void projection(const Matrix44& mat);
    void push(Matrix44& mat);
    void pop();
    Matrix44 mvp();
    Matrix44 mv();
};

class Node {
public:
    void preRender(RenderingContext& ctx);
    void Render(RenderingContext& ctx);
    void postRender(RenderingContext& ctx);
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
class Camera {
public:
    Camera(const ClippingVolume& clippingVolume);
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
protected:
    ClippingVolume clippingVolume;
private:
    Vector3 positionV;
    Vector3 directionV;
    Vector3 rightV;
    Vector3 upV;
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(const ClippingVolume& clippingVolume);
    void  preRender(RenderingContext ctx);
    void render(RenderingContext ctx);
    void  postRender(RenderingContext ctx);
};

class ParallelCamera : public Camera {
public:
    ParallelCamera(const ClippingVolume& clippingVolume);
    void  preRender(RenderingContext& ctx);
    void  postRender(RenderingContext& ctx);
};

class Group : public Node {
public:
    Group();
    void Transformation(const Matrix44& tr);
    void Add(std::shared_ptr<Node> node);
    virtual void Render(RenderingContext& ctx);
protected:
    std::vector<std::shared_ptr<Node>> children;
    Matrix44 transformation;
};

#endif