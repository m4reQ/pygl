#pragma once
#include <stdbool.h>
#include <Python.h>
#include <structmember.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <cglm/vec4.h>

#pragma pack(push, 1)
typedef struct
{
    PyObject_HEAD
        size_t length;
} Vector;
#pragma pack(pop)

typedef struct
{
    PyObject_HEAD
        size_t length;
    vec2 data;
} Vector2;

typedef struct
{
    PyObject_HEAD
        size_t length;
    vec3 data;
} Vector3;

typedef struct
{
    PyObject_HEAD
        size_t length;
    vec4 data;
} Vector4;

extern PyTypeObject pyVector2Type;
extern PyTypeObject pyVector3Type;
extern PyTypeObject pyVector4Type;

bool py_vector_is_vector(PyObject *obj);
void py_vector_copy(void *dst, const Vector *vector);
uint8_t py_vector_size(const Vector *vector);
