#ifndef _matrix_hpp_
#define _matrix_hpp_

struct Vector3 {
    Vector3();
    Vector3(float x, float y, float z);
    float v[3];
    float x() const;
    float y() const;
    float z() const;
};

Vector3 operator+(const Vector3& v1, const Vector3& v2);

Vector3 operator-(const Vector3& v1, const Vector3& v2);

Vector3 operator*(const Vector3& v, float t);

struct Vector4 {
    Vector4(float x, float y, float z, float w);
    Vector4(const Vector3 vec3, float w);
    float v[4];
};

struct Color : Vector4 {
    Color();
    Color(float r, float g, float b, float a = 1.0f);
    float r() const;
    float g() const;
    float b() const;
    float a() const;
};

Vector3 normalize(Vector3 v);

Vector3 crossProduct(const Vector3& u, const Vector3& v);

struct Matrix44
{
    float m[16];
};

Matrix44 Multm(Matrix44 m1, Matrix44 m2);

Matrix44 Identity();

Matrix44 Frustum(float left, float right, float bottom, float top, float nearp, float farp);

Matrix44 Ortho(float left, float right, float bottom, float top, float nearp, float farp);

Matrix44 Translation(float x, float y, float z);

float toRadians(float a);

Matrix44 Rotation(float a, float x, float y, float z);

Matrix44 LookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

#endif
