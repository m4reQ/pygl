#include "texture.h"
#include <glad/gl.h>

static int init(PyTextureSpec *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {
        "target",
        "width",
        "height",
        "internal_format",
        /* optional */
        "depth",        // = 1
        "samples",      // = 1
        "mipmaps",      // = 1
        "min_filter",   // = GL_LINEAR
        "mag_filter",   // = GL_LINEAR
        "wrap_mode",    // = GL_CLAMP_TO_EDGE
        "swizzle_mask", // = NULL
        NULL,
    };

    self->depth = 1;
    self->samples = 1;
    self->mipmaps = 1;
    self->minFilter = GL_LINEAR;
    self->magFilter = GL_LINEAR;
    self->wrapMode = GL_CLAMP_TO_EDGE;
    self->swizzleMask = NULL;

    if (!PyArg_ParseTupleAndKeywords(
            args, kwargs, "IiiI|iiiIIIO", kwNames,
            &self->target,
            &self->width, &self->height, &self->internalFormat,
            &self->depth, &self->samples, &self->mipmaps,
            &self->minFilter, &self->magFilter, &self->wrapMode, &self->swizzleMask))
        return -1;

    Py_XINCREF(self->swizzleMask);

    return 0;
}

static PyObject *get_size(PyTextureSpec *self, void *Py_UNUSED(closure))
{
    return PyTuple_Pack(
        3,
        PyLong_FromLong(self->width),
        PyLong_FromLong(self->height),
        PyLong_FromLong(self->depth));
}

static PyObject *get_is_multisampled(PyTextureSpec *self, void *Py_UNUSED(closure))
{
    return PyBool_FromLong(self->samples > 1);
}

PyTypeObject pyTextureSpecType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.textures.TextureSpec",
    .tp_basicsize = sizeof(PyTextureSpec),
    .tp_init = (initproc)init,
    .tp_members = (PyMemberDef[]){
        {"target", Py_T_UINT, offsetof(PyTextureSpec, target), 0, NULL},
        {"width", Py_T_INT, offsetof(PyTextureSpec, width), 0, NULL},
        {"height", Py_T_INT, offsetof(PyTextureSpec, height), 0, NULL},
        {"depth", Py_T_INT, offsetof(PyTextureSpec, depth), 0, NULL},
        {"samples", Py_T_INT, offsetof(PyTextureSpec, samples), 0, NULL},
        {"mipmaps", Py_T_INT, offsetof(PyTextureSpec, mipmaps), 0, NULL},
        {"internal_format", Py_T_UINT, offsetof(PyTextureSpec, internalFormat), 0, NULL},
        {"min_filter", Py_T_UINT, offsetof(PyTextureSpec, minFilter), 0, NULL},
        {"mag_filter", Py_T_UINT, offsetof(PyTextureSpec, magFilter), 0, NULL},
        {"wrap_mode", Py_T_UINT, offsetof(PyTextureSpec, wrapMode), 0, NULL},
        {0},
    },
    .tp_getset = (PyGetSetDef[]){
        {"size", (getter)get_size, NULL, NULL, NULL},
        {"is_multisampled", (getter)get_is_multisampled, NULL, NULL, NULL},
        {0},
    },
};
