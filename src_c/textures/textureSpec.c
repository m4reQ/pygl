#include "textureSpec.h"
#include <structmember.h>
#include <glad/gl.h>

static int init(PyTextureSpec* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"width", "height", "internal_format", "layers", "samples", "mipmaps", "min_filter", "mag_filter", "wrap_mode", NULL};

    self->layers = 0;
    self->samples = 1;
    self->mipmaps = 4;
    self->minFilter = GL_LINEAR_MIPMAP_LINEAR;
    self->magFilter = GL_LINEAR;
    self->wrapMode = GL_CLAMP_TO_EDGE;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "iiI|iiIII", kwNames,
        &self->width, &self->height, &self->internalFormat,
        &self->layers, &self->samples, &self->mipmaps,
        &self->minFilter, &self->magFilter, &self->wrapMode))
        return -1;

    if (self->samples < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Samples count has to be higher than 0.");
        return -1;
    }

    if (self->mipmaps < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Mipmaps count has to be higher than 0.");
        return -1;
    }

    if (self->width < 0 || self->height < 0 || self->layers < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Texture size has to be higher than 0.");
        return -1;
    }

    return 0;
}

static PyObject* size_getter(PyTextureSpec* self, void* Py_UNUSED(closure))
{
    return PyTuple_Pack(2, PyLong_FromLong(self->width), PyLong_FromLong(self->height));
}

static PyObject* is_multisampled_getter(PyTextureSpec* self, void* Py_UNUSED(closure))
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
        {"width", T_INT, offsetof(PyTextureSpec, width), 0, NULL},
        {"height", T_INT, offsetof(PyTextureSpec, height), 0, NULL},
        {"layers", T_INT, offsetof(PyTextureSpec, layers), 0, NULL},
        {"mipmaps", T_INT, offsetof(PyTextureSpec, mipmaps), 0, NULL},
        {"internal_format", T_UINT, offsetof(PyTextureSpec, internalFormat), 0, NULL},
        {"min_filter", T_UINT, offsetof(PyTextureSpec, minFilter), 0, NULL},
        {"mag_filter", T_UINT, offsetof(PyTextureSpec, magFilter), 0, NULL},
        {"wrap_mode", T_UINT, offsetof(PyTextureSpec, wrapMode), 0, NULL},
        {"samples", T_INT, offsetof(PyTextureSpec, samples), 0, NULL},
        {0}},
    .tp_getset = (PyGetSetDef[]){
        {"size", (getter)size_getter, NULL, NULL, NULL},
        {"is_multisampled", (getter)is_multisampled_getter, NULL, NULL, NULL},
        {0}},
};
