#include "textureBase.h"
#include "textureSpec.h"
#include <structmember.h>

static int py_texture_2d_init(PyTexture* self, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    PyTextureSpec* spec = NULL;
    if (!PyArg_ParseTuple(args, "O!", &pyTextureSpecType, &spec))
        return -1;

    if (!texture_create(self, GL_TEXTURE_2D, spec, true))
        return -1;

    return 0;
}

static PyMethodDef pyTexture2DMethods[] = {
    {"delete", (PyCFunction)py_texture_delete, METH_NOARGS, NULL},
    {"upload", (PyCFunction)py_texture_upload, METH_VARARGS | METH_KEYWORDS, NULL},
    {"set_parameter", (PyCFunction)py_texture_set_parameter, METH_VARARGS},
    {"bind", (PyCFunction)py_texture_bind, METH_NOARGS, NULL},
    {"bind_to_unit", (PyCFunction)py_texture_bind_to_unit, METH_VARARGS, NULL},
    {"bind_textures", (PyCFunction)py_texture_bind_textures, METH_VARARGS | METH_VARARGS | METH_STATIC, NULL},
    {"generate_mipmap", (PyCFunction)py_texture_generate_mipmap, METH_NOARGS, NULL},
    {0},
};

static PyMemberDef pyTexture2DMembers[] = {
    {"width", T_INT, offsetof(PyTexture, width), 0, NULL},
    {"height", T_INT, offsetof(PyTexture, height), 0, NULL},
    {0},
};

PyTypeObject pyTexture2DType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.textures.Texture2D",
    .tp_basicsize = sizeof(PyTexture),
    .tp_dealloc = (destructor)py_texture_dealloc,
    .tp_init = (initproc)py_texture_2d_init,
    .tp_methods = pyTexture2DMethods,
    .tp_members = pyTexture2DMembers,
};
