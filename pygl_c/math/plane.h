#pragma once
#include <Python.h>
#include <cglm/vec3.h>

typedef struct
{
    PyObject_HEAD
    float distance;
    vec3 normal;
} Plane;

extern PyTypeObject pyPlaneType;