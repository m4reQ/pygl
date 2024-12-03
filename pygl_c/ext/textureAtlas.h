#pragma once
#include <Python.h>
#include <glad/gl.h>

typedef struct
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} AtlasEmptyNode;

typedef struct
{
    PyObject ob_base; // PyObject_HEAD
    GLuint textureId;
    AtlasEmptyNode *nodes;
} PyTextureAtlas;

extern PyTypeObject pyTextureAtlasType;
