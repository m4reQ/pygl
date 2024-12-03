#include "buffer.h"
#include "../module.h"

static EnumDef bufferFlagsEnum = {
    .enumName = "BufferFlags",
    .values = (EnumValue[]){
        {"MAP_WRITE_BIT", GL_MAP_WRITE_BIT},
        {"MAP_READ_BIT", GL_MAP_READ_BIT},
        {"MAP_PERSISTENT_BIT", GL_MAP_PERSISTENT_BIT},
        {"MAP_COHERENT_BIT", GL_MAP_COHERENT_BIT},
        {"DYNAMIC_STORAGE_BIT", GL_DYNAMIC_STORAGE_BIT},
        {0},
    },
    .isFlag = true,
};

static EnumDef bufferBaseEnum = {
    .enumName = "BufferBaseTarget",
    .values = (EnumValue[]){
        {"ATOMIC_COUNTER_BUFFER", GL_ATOMIC_COUNTER_BUFFER},
        {"TRANSFORM_FEEDBACK_BUFFER", GL_TRANSFORM_FEEDBACK_BUFFER},
        {"UNIFORM_BUFFER", GL_UNIFORM_BUFFER},
        {"SHADER_STORAGE_BUFFER", GL_SHADER_STORAGE_BUFFER},
        {0},
    },
};

static EnumDef bindTargetEnum = {
    .enumName = "BindTarget",
    .values = (EnumValue[]){
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
    },
};

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.buffers",
        .m_size = -1},
    .enums = (EnumDef *[]){
        &bindTargetEnum,
        &bufferBaseEnum,
        &bufferFlagsEnum,
        NULL,
    },
    .types = (PyTypeObject *[]){
        &pyBufferType,
        NULL,
    },
};

PyMODINIT_FUNC PyInit_buffers(void)
{
    return module_create_from_info(&modInfo);
}
