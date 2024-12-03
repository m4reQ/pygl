#pragma once
#include <stdbool.h>
#include <Python.h>
#include <structmember.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    bool fromSource;
    GLenum type;
    PyObject* pyFilepath;
} PyShaderStage;

extern PyTypeObject pyShaderStageType;
