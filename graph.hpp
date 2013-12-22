#ifndef _graph_hpp_
#define _graph_hpp_

#include <math.h>
#include <vector>
#include <memory>

#include "matrix.hpp"

template<class T>
struct RenderingContext {
    std::vector<Matrix44<T>> mvpStack;
    std::vector<Matrix44<T>> mvStack;
    RenderingContext() {
        mvpStack.push_back(Identity<T>());
        mvStack.push_back(Identity<T>());
    }
    void projection(const Matrix44<T>& mat) {
        mvpStack.push_back(mat);
    }
    void push(Matrix44<T>& mat) {
        mvpStack.push_back(Multm(mvpStack.back(), mat));
        mvStack.push_back(Multm(mvStack.back(), mat));
    }
    Matrix44<T> mvp() {
        return mvpStack.back();
    }
    Matrix44<T> mv() {
        return mvStack.back();
    }
};

template<class T>
struct ClippingVolume {
    ClippingVolume() {}
//    ClippingVolume(T left, T right, T bottom, T top, T nearp, T farp) :
//        left(left), right(right), bottom(bottom), top(top), nearp(nearp), farp(farp) {}
    T left;
    T right;
    T bottom;
    T top;
    T nearp;
    T farp;
};

// cf http://www.codecolony.de/opengl.htm#camera2
template<class T>
class Camera {
public:
    Camera(const ClippingVolume<T>& clippingVolume) : clippingVolume(clippingVolume), positionV(Vector3<T>(0, 0, 0)),
        directionV(Vector3<T>(0, 0, -1)), rightV(Vector3<T>(1, 0, 0)), upV(Vector3<T>(0, 1, 0)) {}
    void reset() {
        positionV = Vector3<T>(0, 0, 0);
        directionV = Vector3<T>(0, 0, -1);
        rightV = Vector3<T>(1, 0, 0);
        upV = Vector3<T>(0, 1, 0);
    }
    void rotateX(T deg) {
        directionV = normalize(directionV * cos(toRadians(deg)) + upV * sin(toRadians(deg)));
        upV = crossProduct(directionV, rightV) * -1;
    }
    void rotateY(T deg) {
        directionV = normalize(directionV * cos(toRadians(deg)) - rightV * sin(toRadians(deg)));
        rightV = crossProduct(directionV, upV);
    }
    void rotateZ(T deg) {
        rightV = normalize(rightV * cos(toRadians(deg)) + upV * sin(toRadians(deg)));
        upV = crossProduct(directionV, rightV) * -1;
    }
    void moveRight(T dist) {
        positionV = positionV + (rightV * dist);
    }
    void moveLeft(T dist) {
        positionV = positionV - (rightV * dist);
    }
    void moveUp(T dist) {
        positionV = positionV + (upV * dist);
    }
    void moveDown(T Float) {
        positionV = positionV - (upV * dist);
    }
    void moveForward(T dist) {
        positionV = positionV + (directionV * dist);
    }
    void moveBackward(T dist) {
        positionV = positionV - (directionV * dist);
    }
    void positionAndOrient() {
        centerV = positionV + directionV;
        context.matrixStack.pushModelView(
            Matrix44.lookAt(positionV.x, positionV.y, positionV.z, centerV.x, centerV.y, centerV.z, upV.x, upV.y, upV.z));
    }
protected:
    ClippingVolume<T> clippingVolume;
private:
    Vector3<T> positionV;
    Vector3<T> directionV;
    Vector3<T> rightV;
    Vector3<T> upV;
};

template<class T>
class PerspectiveCamera : public Camera<T> {
public:
    PerspectiveCamera(const ClippingVolume<T>& clippingVolume) : Camera(clippingVolume) {}
    void  preRender(RenderingContext<T> ctx) {
    }
    void render(RenderingContext<T> ctx) {
    }
    void  postRender(RenderingContext<T> ctx) {
    }
};

template<class T>
class ParallelCamera : public Camera<T> {
public:
    ParallelCamera(const ClippingVolume<T>& clippingVolume) : Camera(clippingVolume) {}
    void  preRender(RenderingContext<T> ctx) {
        auto& cv = clippingVolume;
        mvpStack.push(Ortho<T>(cv.right, cv.left, cv.top, cv.bottom, cv.nearp, cv.farp));
    }
    void render(RenderingContext<T> ctx) {
    }
    void  postRender(RenderingContext<T> ctx) {
        mvpStack.pop();
    }
};

class Node {
public:
    virtual void Render(Matrix44f matrix) = 0;
    void SetParent(std::shared_ptr<Node> parent);
private:
    std::shared_ptr<Node> parent;
};

class Group : public Node {
public:
    void Add(std::shared_ptr<Node> node);
protected:
    std::vector<std::shared_ptr<Node>> children;
};

class Transformation : public Group {
public:
    
private:
    Matrix44f matrix;
};

#endif