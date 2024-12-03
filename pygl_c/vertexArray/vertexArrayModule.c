#include "vertexDescriptor.h"
#include "vertexInput.h"
#include "vertexArray.h"
#include "../module.h"

static EnumDef attribTypeEnum = {
    .enumName = "AttribType",
    .values = (EnumValue[]){
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
    },
};

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.vertex_array",
        .m_size = -1,
    },
    .enums = (EnumDef *[]){
        &attribTypeEnum,
        NULL,
    },
    .types = (PyTypeObject *[]){
        &pyVertexInputType,
        &pyVertexDescriptorType,
        &pyVertexArrayType,
        NULL,
    },
};

PyMODINIT_FUNC PyInit_vertex_array()
{
    return module_create_from_info(&modInfo);
}
