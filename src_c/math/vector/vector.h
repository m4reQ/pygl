#pragma once
#include <Python.h>
#include <structmember.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>

typedef struct
{
    PyObject_HEAD
    vec2 data;
} Vector2;

typedef struct
{
    PyObject_HEAD
    vec3 data;
} Vector3;

typedef struct
{
    PyObject_HEAD
    vec4 data;
} Vector4;

extern PyTypeObject pyVector2Type;
extern PyTypeObject pyVector3Type;
extern PyTypeObject pyVector4Type;
