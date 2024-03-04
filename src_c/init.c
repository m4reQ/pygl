#include <Python.h>
#include <glad/gl.h>
#include <stdbool.h>
#include "utility.h"

static bool isLoaded = false;

static PyObject* py_init(PyObject* Py_UNUSED(self), PyObject* getAddrProc)
{
    if (isLoaded)
        Py_RETURN_NONE;

    int success = gladLoaderLoadGL();
    ThrowIf(
        !success,
        PyExc_RuntimeError,
        "Failed to load OpenGL bindings.",
        NULL);

    isLoaded = true;
    Py_RETURN_NONE;
}

static PyMethodDef moduleMethods[] = {
    {"init", py_init, METH_NOARGS, NULL},
    {0},
};

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.init",
    .m_methods = moduleMethods,
};

PyMODINIT_FUNC PyInit_init()
{
    return PyModule_Create(&moduleDef);
}
