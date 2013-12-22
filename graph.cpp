#include <math.h>
#include <vector>
#include <memory>

#include "matrix.hpp"
#include "graph.hpp"

RenderingContext::RenderingContext() {
    mvpStack.push_back(Identity());
    mvStack.push_back(Identity());
}

void RenderingContext::projection(const Matrix44& mat) {
    mvpStack.push_back(mat);
}

void RenderingContext::push(Matrix44& mat) {
    mvpStack.push_back(Multm(mvpStack.back(), mat));
    mvStack.push_back(Multm(mvStack.back(), mat));
}

void RenderingContext::pop() {
    mvpStack.pop_back();
    mvStack.pop_back();
}

Matrix44 RenderingContext::mvp() {
    return mvpStack.back();
}

Matrix44 RenderingContext::mv() {
    return mvStack.back();
}

void Node::preRender(RenderingContext& ctx) {}
void Node::Render(RenderingContext& ctx) {}
void Node::postRender(RenderingContext& ctx) {}

Camera::Camera(const ClippingVolume& clippingVolume) : clippingVolume(clippingVolume), positionV(Vector3(0, 0, 0)),
        directionV(Vector3(0, 0, -1)), rightV(Vector3(1, 0, 0)), upV(Vector3(0, 1, 0)) {}

void Camera::reset() {
    positionV = Vector3(0, 0, 0);
    directionV = Vector3(0, 0, -1);
    rightV = Vector3(1, 0, 0);
    upV = Vector3(0, 1, 0);
}

void Camera::rotateX(float deg) {
    directionV = normalize(directionV * cos(toRadians(deg)) + upV * sin(toRadians(deg)));
    upV = crossProduct(directionV, rightV) * -1;
}

void Camera::rotateY(float deg) {
    directionV = normalize(directionV * cos(toRadians(deg)) - rightV * sin(toRadians(deg)));
    rightV = crossProduct(directionV, upV);
}

void Camera::rotateZ(float deg) {
    rightV = normalize(rightV * cos(toRadians(deg)) + upV * sin(toRadians(deg)));
    upV = crossProduct(directionV, rightV) * -1;
}

void Camera::moveRight(float dist) {
    positionV = positionV + (rightV * dist);
}

void Camera::moveLeft(float dist) {
    positionV = positionV - (rightV * dist);
}

void Camera::moveUp(float dist) {
    positionV = positionV + (upV * dist);
}

void Camera::moveDown(float dist) {
    positionV = positionV - (upV * dist);
}

void Camera::moveForward(float dist) {
    positionV = positionV + (directionV * dist);
}

void Camera::moveBackward(float dist) {
    positionV = positionV - (directionV * dist);
}

Matrix44 Camera::positionAndOrient() {
    Vector3 centerV = positionV + directionV;
    return LookAt(positionV.x(), positionV.y(), positionV.z(), centerV.x(), centerV.y(), centerV.z(), upV.x(), upV.y(), upV.z());
}

PerspectiveCamera::PerspectiveCamera(const ClippingVolume& clippingVolume) : Camera(clippingVolume) {}

void PerspectiveCamera::preRender(RenderingContext ctx) {
}

void PerspectiveCamera::render(RenderingContext ctx) {
}

void PerspectiveCamera::postRender(RenderingContext ctx) {
}

ParallelCamera::ParallelCamera(const ClippingVolume& clippingVolume) : Camera(clippingVolume) {}

void ParallelCamera::preRender(RenderingContext& ctx) {
    auto& cv = clippingVolume;
    ctx.push(Ortho(cv.right, cv.left, cv.top, cv.bottom, cv.nearp, cv.farp));
}

void ParallelCamera::postRender(RenderingContext& ctx) {
    ctx.pop();
}

Group::Group() : transformation(Identity()) {}

void Group::Transformation(const Matrix44& tr) { transformation = tr; }

void Group::Add(std::shared_ptr<Node> node) { children.push_back(node); }

void Group::Render(RenderingContext& ctx) {
    ctx.push(transformation);
    for (auto child : children) {
        child->Render(ctx);
    }
    ctx.pop();
}
