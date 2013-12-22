#include <math.h>

#include "matrix.hpp"

Vector3::Vector3(float x, float y, float z) { v[0] = x; v[1] = y; v[2] = z; }
float Vector3::x() const { return v[0]; }
float Vector3::y() const { return v[1]; }
float Vector3::z() const { return v[2]; }

Vector3 operator+(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1.v[0] + v2.v[0], v1.v[1] + v2.v[1], v1.v[2] + v2.v[2]);
}

Vector3 operator-(const Vector3& v1, const Vector3& v2) {
    return Vector3(v1.v[0] - v2.v[0], v1.v[1] - v2.v[1], v1.v[2] - v2.v[2]);
}

Vector3 operator*(const Vector3& v, float t) {
    return Vector3(t*v.v[0], t*v.v[1], t*v.v[2]);
}

Vector4::Vector4(float x, float y, float z, float w) { v[0] = x; v[1] = y; v[2] = z; v[3] = w; }
Vector4::Vector4(const Vector3 vec3, float w) { v[0] = vec3.v[0]; v[1] = vec3.v[1]; v[2] = vec3.v[3]; v[3] = w; }

Color::Color(float r, float g, float b, float a) : Vector4(r, g, b, a) {};
float Color::r() const { return v[0]; }
float Color::g() const { return v[1]; }
float Color::b() const { return v[2]; }
float Color::a() const { return v[3]; }

Vector3 normalize(Vector3 v) {
    float norm = sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
    return Vector3(v.v[0] / norm, v.v[0] / norm, v.v[0] / norm);
}

Vector3 crossProduct(const Vector3& u, const Vector3& v) {
    return Vector3(u.y()*v.z() - u.z()*v.y(), u.z()*v.x() - u.x()*v.z(), u.x()*v.y() - u.y()*v.x());
}

Matrix44 Multm(Matrix44& m1, Matrix44& m2) {
    Matrix44 m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i + j * 4] =
                m1.m[i + 0] * m2.m[j * 4 + 0] +
                m1.m[i + 4] * m2.m[j * 4 + 1] +
                m1.m[i + 8] * m2.m[j * 4 + 2] +
                m1.m[i + 12] * m2.m[j * 4 + 3];
        }
    }
    return m;
}

template <class... M>
Matrix44 Multm(Matrix44& m1, Matrix44& m2, M&... m) {
    return Multm(m1, Multm(m2, m...));
};

Matrix44 Identity() {
    Matrix44 mat;
    mat.m[0] = 1;
    mat.m[1] = 0;
    mat.m[2] = 0;
    mat.m[3] = 0;
    mat.m[4] = 0;
    mat.m[5] = 1;
    mat.m[6] = 0;
    mat.m[7] = 0;
    mat.m[8] = 0;
    mat.m[9] = 0;
    mat.m[10] = 1;
    mat.m[11] = 0;
    mat.m[12] = 0;
    mat.m[13] = 0;
    mat.m[14] = 0;
    mat.m[15] = 1;
    return mat;
}

Matrix44 Ortho(float right, float left, float top, float bottom, float nearp, float farp) {
    Matrix44 mat;
    mat.m[0] = 2 / (right - left);
    mat.m[1] = 0.0f;
    mat.m[2] = 0.0f;
    mat.m[3] = 0.0f;
    mat.m[4] = 0.0f;
    mat.m[5] = 2 / (top - bottom);
    mat.m[6] = 0.0f;
    mat.m[7] = 0.0f;
    mat.m[8] = 0.0f;
    mat.m[9] = 0.0f;
    mat.m[10] = 2 / (farp - nearp);
    mat.m[11] = 0.0f;
    mat.m[12] = -(right + left) / (right - left);
    mat.m[13] = -(top + bottom) / (top - bottom);
    mat.m[14] = -(farp + nearp) / (farp - nearp);
    mat.m[15] = 1.0f;
    return mat;
}

Matrix44 Translation(float x, float y, float z) {
    Matrix44 mat;
    mat.m[0] = 1.0f;
    mat.m[1] = 0.0f;
    mat.m[2] = 0.0f;
    mat.m[3] = 0.0f;
    mat.m[4] = 0.0f;
    mat.m[5] = 1.0f;
    mat.m[6] = 0.0f;
    mat.m[7] = 0.0f;
    mat.m[8] = 0.0f;
    mat.m[9] = 0.0f;
    mat.m[10] = 1.0f;
    mat.m[11] = 0.0f;
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = z;
    mat.m[15] = 1.0f;
    return mat;
}

float toRadians(float a) {
    return a * 3.141592653589793 / 180.0;
}

Matrix44 Rotation(float a, float x, float y, float z) {
    Matrix44 mat;
    float c = (float)cos(toRadians(a));
    float s = (float)sin(toRadians(a));
    mat.m[0] = x * x * (1 - c) + c;
    mat.m[1] = y * x * (1 - c) + z * s;
    mat.m[2] = x * z * (1 - c) - y * s;
    mat.m[3] = 0.0f;
    mat.m[4] = y * x * (1 - c) - z * s;
    mat.m[5] = y * y * (1 - c) + c;
    mat.m[6] = y * z * (1 - c) + x * s;
    mat.m[7] = 0.0f;
    mat.m[8] = x * z * (1 - c) + y * s;
    mat.m[9] = y * z * (1 - c) - x * s;
    mat.m[10] = z * z * (1 - c) + c;
    mat.m[11] = 0.0f;
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
    return mat;
}

// cf gluLookAt http://www.unix.com/man-page/All/3/gluLookAt/
Matrix44 LookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) {
    auto f = normalize(Vector3(centerX - eyeX, centerY - eyeY, centerZ - eyeZ));
    auto up = normalize(Vector3(upX, upY, upZ));
    auto s = crossProduct(f, up);
    auto u = crossProduct(s, f);
    Matrix44 m44;
    m44.m[0] = s.x();
    m44.m[1] = u.x();
    m44.m[2] = -f.x();
    m44.m[3] = 0;
    m44.m[4] = s.y();
    m44.m[5] = u.y();
    m44.m[6] = -f.y();
    m44.m[7] = 0;
    m44.m[8] = s.z();
    m44.m[9] = u.z();
    m44.m[10] = -f.z();
    m44.m[11] = 0;
    m44.m[12] = 0;
    m44.m[13] = 0;
    m44.m[14] = 0;
    m44.m[15] = 1;
    return Multm(m44, Translation(-eyeX, -eyeY, -eyeZ));
}
