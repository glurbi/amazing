#ifndef _matrix_hpp_
#define _matrix_hpp_

template<class T>
struct Vector3 {
    Vector3(T x, T y, T z) { v[0] = x; v[1] = y; v[2] = z; }
    T v[3];
    T x() { return v[0] };
    T y() { return v[1] };
    T z() { return v[2] };
};

template<class T>
Vector3<T> operator+(const Vector3<T>& v1, const Vector3<T>& v2) {
    return Vector3<T>(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

template<class T>
Vector3<T> operator-(const Vector3<T>& v1, const Vector3<T>& v2) {
    return Vector3<T>(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

template<class T>
Vector3<T> operator*(const Vector3<T>& v, T t) {
    return Vector3<T>(t*v[0], t*v[1], t*v[2]);
}

template <class T>
struct Vector4 {
    Vector4(T x, T y, T z, T w) { v[0] = x; v[1] = y; v[2] = z; v[3] = w; }
    Vector4(const Vector3<T> vec3, T w) { v[0] = vec3.v[0]; v[1] = vec3.v[1]; v[2] = vec3.v[3]; v[3] = w; }
    T v[4];
};

struct Color : Vector4<float> {
    Color(float r, float g, float b, float a = 1.0f) : Vector4(r, g, b, a) {};
    inline float r() const { return v[0]; }
    inline float g() const { return v[1]; }
    inline float b() const { return v[2]; }
    inline float a() const { return v[3]; }
};

template<class T>
Vector3<T> normalize(Vector3<T> v) {
    T norm = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    return Vector3D<T>(v[0] / norm, v[0] / norm, v[0] / norm);
}

template<class T>
Vector3<T> crossProduct(const Vector3<T>& u, const Vector3<T>& v) {
    return Vector3<T>(u.y*v.z - u.z*v.y, u.z*v.x - u.x*v.z, u.x*v.y - u.y*v.x);
}

template <class T>
struct Matrix44
{
    T m[16];
};

template <class T>
Matrix44<T> Identity() {
    Matrix44<T> mat;
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


template <class T>
Matrix44<T> Ortho(T right, T left, T top, T bottom, T nearp, T farp) {
    Matrix44<T> mat;
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

template <class T>
Matrix44<T> Translation(T x, T y, T z) {
    Matrix44<T> mat;
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

template<class T>
T toRadians(T a) {
    return a * (T)3.141592653589793 / (T)180.0;
}

template <class T>
Matrix44<T> Rotation(T a, T x, T y, T z) {
    Matrix44<T> mat;
    float c = (float) cos(toRadians(a));
    float s = (float) sin(toRadians(a));
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
template<class T>
Matrix44<T> LookAt(T eyeX, T eyeY, T eyeZ, T centerX, T centerY, T centerZ, T upX, T upY, T upZ) {
    auto f = normalize(Vector3<T>(centerX - eyeX, centerY - eyeY, centerZ - eyeZ));
    auto up = normalize(Vector3<T>(upX, upY, upZ));
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

template <class T>
Matrix44<T> Multm(Matrix44<T>& m1, Matrix44<T>& m2) {
    Matrix44<T> m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i+j*4] =
                m1.m[i+0] * m2.m[j*4+0] +
                m1.m[i+4] * m2.m[j*4+1] +
                m1.m[i+8] * m2.m[j*4+2] +
                m1.m[i+12] * m2.m[j*4+3];
        }
    }
    return m;
}

template <class T, class... N>
Matrix44<T> Multm(Matrix44<T>& m1, Matrix44<T>& m2, N&... n) {
    return Multm(m1, Multm(m2, n...));
};

typedef Matrix44<float> Matrix44f;

#endif