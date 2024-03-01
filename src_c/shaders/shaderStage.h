#pragma once
#include <Python.h>
#include <structmember.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    GLenum type;
    PyObject* pyFilepath;
} PyShaderStage;

extern PyTypeObject pyShaderStageType;
