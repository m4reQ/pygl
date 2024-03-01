#include "shaderStage.h"

static int py_shaderstage_init(PyShaderStage* self, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    if (!PyArg_ParseTuple(args, "IU", &self->type, &self->pyFilepath))
        return -1;

    Py_INCREF(self->pyFilepath);
    return 0;
}

static void py_shaderstage_dealloc(PyShaderStage* self)
{
    Py_CLEAR(self->pyFilepath);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* py_shaderstage_repr(PyShaderStage* self)
{
    return PyUnicode_FromFormat(
        "<%s [type: %d, filepath: %U] at %X>",
        Py_TYPE(self)->tp_name,
        self->type,
        self->pyFilepath,
        (intptr_t)self);
}

static PyMemberDef pyShaderStageMembers[] = {
    {"type", T_UINT, offsetof(PyShaderStage, type), 0, NULL},
    {"filepath", T_OBJECT_EX, offsetof(PyShaderStage, pyFilepath), 0, NULL},
    {0},
};

PyTypeObject pyShaderStageType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.shaders.ShaderStage",
    .tp_basicsize = sizeof(PyShaderStage),
    .tp_members = pyShaderStageMembers,
    .tp_init = py_shaderstage_init,
    .tp_dealloc = py_shaderstage_dealloc,
    .tp_repr = py_shaderstage_repr,
};
