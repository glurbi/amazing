#ifndef _matrix_hpp_
#define _matrix_hpp_

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

#endif _matrix_hpp_