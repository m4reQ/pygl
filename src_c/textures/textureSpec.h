#pragma once
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    GLsizei width, height, layers;
    GLsizei samples;
    GLsizei mipmaps;
    GLenum internalFormat;
    GLenum minFilter, magFilter;
    GLenum wrapMode;
} PyTextureSpec;

extern PyTypeObject pyTextureSpecType;
