#ifndef _graph_hpp_
#define _graph_hpp_

#include <math.h>
#include <vector>
#include <memory>

#include "matrix.hpp"

struct RenderingContext {
    std::vector<Matrix44> mvpStack;
    std::vector<Matrix44> mvStack;
    RenderingContext() {
        mvpStack.push_back(Identity());
        mvStack.push_back(Identity());
    }
    void projection(const Matrix44& mat) {
        mvpStack.push_back(mat);
    }
    void push(Matrix44& mat) {
        mvpStack.push_back(Multm(mvpStack.back(), mat));
        mvStack.push_back(Multm(mvStack.back(), mat));
    }
    void pop() {
        mvpStack.pop_back();
        mvStack.pop_back();
    }
    Matrix44 mvp() {
        return mvpStack.back();
    }
    Matrix44 mv() {
        return mvStack.back();
    }
};

class Node {
public:
    void preRender(RenderingContext& ctx) {}
    void Render(RenderingContext& ctx) {}
    void postRender(RenderingContext& ctx) {}
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
    Camera(const ClippingVolume& clippingVolume) : clippingVolume(clippingVolume), positionV(Vector3(0, 0, 0)),
        directionV(Vector3(0, 0, -1)), rightV(Vector3(1, 0, 0)), upV(Vector3(0, 1, 0)) {}
    void reset() {
        positionV = Vector3(0, 0, 0);
        directionV = Vector3(0, 0, -1);
        rightV = Vector3(1, 0, 0);
        upV = Vector3(0, 1, 0);
    }
    void rotateX(float deg) {
        directionV = normalize(directionV * cos(toRadians(deg)) + upV * sin(toRadians(deg)));
        upV = crossProduct(directionV, rightV) * -1;
    }
    void rotateY(float deg) {
        directionV = normalize(directionV * cos(toRadians(deg)) - rightV * sin(toRadians(deg)));
        rightV = crossProduct(directionV, upV);
    }
    void rotateZ(float deg) {
        rightV = normalize(rightV * cos(toRadians(deg)) + upV * sin(toRadians(deg)));
        upV = crossProduct(directionV, rightV) * -1;
    }
    void moveRight(float dist) {
        positionV = positionV + (rightV * dist);
    }
    void moveLeft(float dist) {
        positionV = positionV - (rightV * dist);
    }
    void moveUp(float dist) {
        positionV = positionV + (upV * dist);
    }
    void moveDown(float dist) {
        positionV = positionV - (upV * dist);
    }
    void moveForward(float dist) {
        positionV = positionV + (directionV * dist);
    }
    void moveBackward(float dist) {
        positionV = positionV - (directionV * dist);
    }
    Matrix44 positionAndOrient() {
        Vector3 centerV = positionV + directionV;
        return LookAt(positionV.x(), positionV.y(), positionV.z(), centerV.x(), centerV.y(), centerV.z(), upV.x(), upV.y(), upV.z());
    }
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
    PerspectiveCamera(const ClippingVolume& clippingVolume) : Camera(clippingVolume) {}
    void  preRender(RenderingContext ctx) {
    }
    void render(RenderingContext ctx) {
    }
    void  postRender(RenderingContext ctx) {
    }
};

class ParallelCamera : public Camera {
public:
    ParallelCamera(const ClippingVolume& clippingVolume) : Camera(clippingVolume) {}
    void  preRender(RenderingContext& ctx) {
        auto& cv = clippingVolume;
        ctx.push(Ortho(cv.right, cv.left, cv.top, cv.bottom, cv.nearp, cv.farp));
    }
    void  postRender(RenderingContext& ctx) {
        ctx.pop();
    }
};

class Group : public Node {
public:
    Group() : transformation(Identity()) {}
    void Transformation(const Matrix44& tr) { transformation = tr; };
    void Add(std::shared_ptr<Node> node) { children.push_back(node); }
    virtual void Render(RenderingContext& ctx) {
        ctx.push(transformation);
        for (auto child : children) {
            child->Render(ctx);
        }
        ctx.pop();
    }
protected:
    std::vector<std::shared_ptr<Node>> children;
    Matrix44 transformation;
};

#endif