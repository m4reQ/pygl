#include "buffer.h"
#include "mappedBuffer.h"

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.buffers",
};

PyMODINIT_FUNC PyInit_buffers(void)
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return NULL;

    if (PyModule_AddType(mod, &pyBufferType))
        return NULL;

    if (PyModule_AddType(mod, &pyMappedBufferType))
        return NULL;

    return mod;
}
