#pragma once
#include <Python.h>
#include <stdbool.h>
#include "../gl.h"

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
    PyObject* swizzleMask;
} PyTextureSpec;

typedef struct
{
    PyObject_HEAD
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    GLint xOffset;
    GLint yOffset;
    GLint zOffset;
    GLint level;
    GLint alignment;
    GLenum format;
    GLenum pixelType;
    Py_ssize_t dataOffset;
    GLsizei imageSize;
    bool generateMipmap;
} PyTextureUploadInfo;

typedef struct
{
    PyObject_HEAD
    GLuint id;
    GLenum target;
    GLenum internalFormat;
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    GLsizei mipmaps;
} PyTexture;

extern PyTypeObject pyTextureSpecType;
extern PyTypeObject pyTextureUploadInfoType;
extern PyTypeObject pyTextureType;
