#include "buffer.h"
#include "../enum.h"

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.buffers",
};

static EnumValue bufferFlagsValues[] = {
    {"MAP_WRITE_BIT", GL_MAP_WRITE_BIT},
    {"MAP_READ_BIT", GL_MAP_READ_BIT},
    {"MAP_PERSISTENT_BIT", GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT},
    {"DYNAMIC_STORAGE_BIT", GL_DYNAMIC_STORAGE_BIT},
    {0},
};
static EnumValue bufferBaseValues[] = {
    {"ATOMIC_COUNTER_BUFFER", GL_ATOMIC_COUNTER_BUFFER},
    {"TRANSFORM_FEEDBACK_BUFFER", GL_TRANSFORM_FEEDBACK_BUFFER},
    {"UNIFORM_BUFFER", GL_UNIFORM_BUFFER},
    {"SHADER_STORAGE_BUFFER", GL_SHADER_STORAGE_BUFFER},
    {0},
};
static EnumValue bindTargetValues[] = {
    {"ARRAY_BUFFER", GL_ARRAY_BUFFER},
    {"ATOMIC_COUNTER_BUFFER", GL_ATOMIC_COUNTER_BUFFER},
    {"COPY_READ_BUFFER", GL_COPY_READ_BUFFER},
    {"COPY_WRITE_BUFFER", GL_COPY_WRITE_BUFFER},
    {"DISPATCH_INDIRECT_BUFFER", GL_DISPATCH_INDIRECT_BUFFER},
    {"DRAW_INDIRECT_BUFFER", GL_DRAW_INDIRECT_BUFFER},
    {"ELEMENT_ARRAY_BUFFER", GL_ELEMENT_ARRAY_BUFFER},
    {"PIXEL_PACK_BUFFER", GL_PIXEL_PACK_BUFFER},
    {"PIXEL_UNPACK_BUFFER", GL_PIXEL_UNPACK_BUFFER},
    {"QUERY_BUFFER", GL_QUERY_BUFFER},
    {"SHADER_STORAGE_BUFFER", GL_SHADER_STORAGE_BUFFER},
    {"TEXTURE_BUFFER", GL_TEXTURE_BUFFER},
    {"TRANSFORM_FEEDBACK_BUFFER", GL_TRANSFORM_FEEDBACK_BUFFER},
    {"UNIFORM_BUFFER", GL_UNIFORM_BUFFER},
    {0},
};

PyMODINIT_FUNC PyInit_buffers(void)
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return NULL;

    if (PyModule_AddType(mod, &pyBufferType))
        return NULL;

    if (!enum_add(mod, "BufferFlags", bufferFlagsValues))
        return NULL;

    if (!enum_add(mod, "BufferBaseTarget", bufferBaseValues))
        return NULL;

    if (!enum_add(mod, "BindTarget", bindTargetValues))
        return NULL;

    return mod;
}
