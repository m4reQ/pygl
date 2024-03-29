#include "textureBase.h"
#include "textureSpec.h"
#include <structmember.h>

static int py_texture_2d_init(PyTexture* self, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    PyTextureSpec* spec = NULL;
    if (!PyArg_ParseTuple(args, "O!", &pyTextureSpecType, &spec))
        return -1;

    GLenum target = spec->samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    if (!texture_create(self, target, spec, true))
        return -1;

    return 0;
}

PyTypeObject pyTexture2DType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.textures.Texture2D",
    .tp_basicsize = sizeof(PyTexture),
    .tp_dealloc = (destructor)py_texture_dealloc,
    .tp_init = (initproc)py_texture_2d_init,
    .tp_methods = pyTextureMethods,
    .tp_members = (PyMemberDef[]){
        {"id", T_UINT, offsetof(PyTexture, id), 0, NULL},
        {"width", T_INT, offsetof(PyTexture, width), 0, NULL},
        {"height", T_INT, offsetof(PyTexture, height), 0, NULL},
        {0}},
};
