#pragma once
#include <Python.h>
#include <glad/gl.h>

#define GL_OBJECT_HEAD GLuint id

typedef struct
{
    PyObject_HEAD
    GL_OBJECT_HEAD;
} GLObject;
