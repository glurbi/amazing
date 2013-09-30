#ifndef _matrix_hpp_
#define _matrix_hpp_

template <class T>
struct Vector4 {
   T v[4];
};

struct Color : Vector4<float> {
    Color(float r, float g, float b, float a = 1.0f)
        { v[0] = r; v[1] = g; v[2] = b; v[3] = a; }
    inline float r() const { return v[0]; }
    inline float g() const { return v[1]; }
    inline float b() const { return v[2]; }
    inline float a() const { return v[3]; }
};

template <class T>
struct Matrix44
{
    T m[16];
};

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

#endif