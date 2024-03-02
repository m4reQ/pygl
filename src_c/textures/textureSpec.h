#pragma once
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    GLsizei width, height;
    GLsizei samples;
    GLsizei mipmaps;
    GLenum internalFormat;
    GLenum minFilter, magFilter;
    GLenum wrapMode;
} PyTextureSpec;

extern PyTypeObject pyTextureSpecType;
