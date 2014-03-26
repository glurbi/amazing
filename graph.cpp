#include <math.h>
#include <vector>
#include <memory>

#include "matrix.hpp"
#include "graph.hpp"
#include "program.hpp"

rendering_context::rendering_context() {
    last_frame_time_seconds = 0.0;
    elapsed_time_seconds = 0.0;
    reset();
}

void rendering_context::projection(Matrix44 mat) {
    mvpStack.push_back(Multm(mvpStack.back(), mat));
}

void rendering_context::push(Matrix44 mat) {
    mvpStack.push_back(Multm(mvpStack.back(), mat));
    mvStack.push_back(Multm(mvStack.back(), mat));
}

void rendering_context::pop() {
    mvpStack.pop_back();
    mvStack.pop_back();
}

void rendering_context::reset() {
    mvpStack.clear();
    mvStack.clear();
    mvpStack.push_back(Identity());
    mvStack.push_back(Identity());
}

Matrix44 rendering_context::mvp() {
    return mvpStack.back();
}

Matrix44 rendering_context::mv() {
    return mvStack.back();
}

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

void PerspectiveCamera::Render(std::shared_ptr<Node> node, rendering_context& ctx, std::shared_ptr<Program> program) {
    auto& cv = clippingVolume;
    ctx.projection(Frustum(cv.left, cv.right, cv.bottom, cv.top, cv.nearp, cv.farp));
    ctx.push(positionAndOrient());
    ctx.program = program;
    node->Render(ctx);
    ctx.reset();
}

ParallelCamera::ParallelCamera(const ClippingVolume& clippingVolume) : Camera(clippingVolume) {}

void ParallelCamera::Render(std::shared_ptr<Node> node, rendering_context& ctx, std::shared_ptr<Program> program) {
    auto& cv = clippingVolume;
    ctx.projection(Ortho(cv.left, cv.right, cv.bottom, cv.top, cv.nearp, cv.farp));
    ctx.push(positionAndOrient());
    ctx.program = program;
    node->Render(ctx);
    ctx.reset();
}

Group::Group() : transformation(Identity()) {}

void Group::Transformation(const Matrix44& tr) { transformation = tr; }

void Group::Add(std::shared_ptr<Node> node) { children.push_back(node); }

void Group::Render(rendering_context& ctx) {
    ctx.push(transformation);
    for (auto child : children) {
        child->Render(ctx);
    }
    ctx.pop();
}

