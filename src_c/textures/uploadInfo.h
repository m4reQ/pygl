#pragma once
#include <stdbool.h>
#include <Python.h>
#include <glad/gl.h>

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
    GLenum format;
    GLenum pixelType;
    size_t dataOffset;
    GLsizei imageSize;
    bool generateMipmap;
} PyUploadInfo;

extern PyTypeObject pyUploadInfoType;
