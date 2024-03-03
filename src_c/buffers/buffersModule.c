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

PyMODINIT_FUNC PyInit_buffers(void)
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return NULL;

    if (PyModule_AddType(mod, &pyBufferType))
        return NULL;

    if (!enum_add(mod, "BufferFlags", bufferFlagsValues))
        return NULL;

    return mod;
}
