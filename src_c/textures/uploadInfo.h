#pragma once
#include <stdbool.h>
#include <Python.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    GLsizei width, height;
    GLenum format, type;
    GLint level, xOffset, yOffset, layer;
    GLsizei imageSize;
    bool generateMipmap;
    size_t dataOffset;
} PyUploadInfo;

extern PyTypeObject pyUploadInfoType;
