#include "shaderStage.h"

static int py_shaderstage_init(PyShaderStage *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"type", "filepath", "from_source", NULL};

    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "IU|p", kwNames,
            &self->type, &self->pyFilepath, &self->fromSource))
        return -1;

    Py_INCREF(self->pyFilepath);
    return 0;
}

static void py_shaderstage_dealloc(PyShaderStage *self)
{
    Py_CLEAR(self->pyFilepath);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *py_shaderstage_repr(PyShaderStage *self)
{
    return PyUnicode_FromFormat(
        "<%s [type: %d, filepath: %s] at %zX>",
        Py_TYPE(self)->tp_name,
        self->type,
        self->fromSource ? "None" : PyUnicode_AsUTF8(self->pyFilepath),
        (intptr_t)self);
}

PyTypeObject pyShaderStageType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.shaders.ShaderStage",
    .tp_basicsize = sizeof(PyShaderStage),
    .tp_members = (PyMemberDef[]){
        {"type", T_UINT, offsetof(PyShaderStage, type), 0, NULL},
        {"filepath", T_OBJECT_EX, offsetof(PyShaderStage, pyFilepath), 0, NULL},
        {"from_source", T_BOOL, offsetof(PyShaderStage, fromSource), 0, NULL},
        {0},
    },
    .tp_init = (initproc)py_shaderstage_init,
    .tp_dealloc = (destructor)py_shaderstage_dealloc,
    .tp_repr = (reprfunc)py_shaderstage_repr,
};
