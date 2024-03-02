#pragma once
#include <stdbool.h>
#include <Python.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    GLsizei width, height;
    GLenum format, type;
    GLint level, xOffset, yOffset;
    GLsizei imageSize;
    bool generateMipmap; // losing 8 bytes because of this 1 byte long field LOL
} PyUploadInfo;

extern PyTypeObject pyUploadInfoType;
