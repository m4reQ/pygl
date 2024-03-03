#include "texture2DArray.h"
#include "textureBase.h"
#include <structmember.h>

static int init(PyTexture* self, PyObject* args, PyObject* Py_UNUSED(kwargs))
{
    PyTextureSpec* spec = NULL;
    if (!PyArg_ParseTuple(args, "O!", &pyTextureSpecType, &spec))
        return -1;

    GLenum target = spec->samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_ARRAY;
    return texture_create(self, target, spec, true) ? 0 : -1;
}

PyTypeObject pyTexture2DArrayType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.textures.Texture2DArray",
    .tp_basicsize = sizeof(PyTexture),
    .tp_dealloc = (destructor)py_texture_dealloc,
    .tp_init = (initproc)init,
    .tp_methods = pyTextureMethods,
    .tp_members = (PyMemberDef[]){
        {"id", T_UINT, offsetof(PyTexture, id), 0, NULL},
        {"width", T_INT, offsetof(PyTexture, width), 0, NULL},
        {"height", T_INT, offsetof(PyTexture, height), 0, NULL},
        {"layers", T_INT, offsetof(PyTexture, layers), 0, NULL},
        {0}},
};
