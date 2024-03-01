#include "vertexDescriptor.h"
#include "vertexInput.h"
#include "vertexArray.h"
#include "../enum.h"

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.vertex_array"
};

PyMODINIT_FUNC PyInit_vertex_array()
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return mod;

    if (PyModule_AddType(mod, &pyVertexInputType))
        return NULL;

    if (PyModule_AddType(mod, &pyVertexDescriptorType))
        return NULL;

    if (PyModule_AddType(mod, &pyVertexArrayType))
        return NULL;

    EnumValue values[] = {
        {"FLOAT", GL_FLOAT},
        {"HALF_FLOAT", GL_HALF_FLOAT},
        {"DOUBLE", GL_DOUBLE},
        {"BYTE", GL_BYTE},
        {"UNSIGNED_BYTE", GL_UNSIGNED_BYTE},
        {"SHORT", GL_SHORT},
        {"UNSIGNED_SHORT", GL_UNSIGNED_SHORT},
        {"INT", GL_INT},
        {"UNSIGNED_INT", GL_UNSIGNED_INT},
        {0},
    };
    if (!enum_add(mod, "AttribType", values))
        return NULL;

    return mod;
}
