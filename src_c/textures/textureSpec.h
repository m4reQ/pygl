#pragma once
#include <Python.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    GLenum target;
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    GLsizei samples;
    GLsizei mipmaps;
    GLenum internalFormat;
    GLenum minFilter;
    GLenum magFilter;
    GLenum wrapMode;
} PyTextureSpec;

extern PyTypeObject pyTextureSpecType;
