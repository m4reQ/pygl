#include "textureSpec.h"
#include <structmember.h>
#include <glad/gl.h>

static int py_texture_spec_init(PyTextureSpec* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"width", "height", "internal_format", "samples", "mipmaps", "min_filter", "mag_filter", "wrap_mode", NULL};

    self->samples = 1;
    self->mipmaps = 4;
    self->minFilter = GL_LINEAR_MIPMAP_LINEAR;
    self->magFilter = GL_LINEAR;
    self->wrapMode = GL_CLAMP_TO_EDGE;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "iiI|iiIII", kwNames,
        &self->width, &self->height, &self->internalFormat,
        &self->samples, &self->mipmaps, &self->minFilter,
        &self->magFilter, &self->wrapMode))
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

    if (self->width < 0 || self->height < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Texture size has to be higher than 0.");
        return -1;
    }

    return 0;
}

static PyObject* py_texture_spec_size_get(PyTextureSpec* self, void* Py_UNUSED(closure))
{
    return Py_BuildValue("(ii)", self->width, self->height);
}

static PyObject* py_texture_spec_is_multisampled_get(PyTextureSpec* self, void* Py_UNUSED(closure))
{
    return PyBool_FromLong(self->samples > 1);
}

static PyMemberDef pyTextureSpecMembers[] = {
    {"width", T_INT, offsetof(PyTextureSpec, width), 0, NULL},
    {"height", T_INT, offsetof(PyTextureSpec, height), 0, NULL},
    {"mipmaps", T_INT, offsetof(PyTextureSpec, mipmaps), 0, NULL},
    {"internal_format", T_UINT, offsetof(PyTextureSpec, internalFormat), 0, NULL},
    {"min_filter", T_UINT, offsetof(PyTextureSpec, minFilter), 0, NULL},
    {"mag_filter", T_UINT, offsetof(PyTextureSpec, magFilter), 0, NULL},
    {"wrap_mode", T_UINT, offsetof(PyTextureSpec, wrapMode), 0, NULL},
    {"samples", T_INT, offsetof(PyTextureSpec, samples), 0, NULL},
    {0},
};

static PyGetSetDef pyTextureSpecGetSet[] = {
    {"size", (getter)py_texture_spec_size_get, NULL, NULL, NULL},
    {"is_multisampled", (getter)py_texture_spec_is_multisampled_get, NULL, NULL, NULL},
    {0},
};

PyTypeObject pyTextureSpecType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.textures.TextureSpec",
    .tp_basicsize = sizeof(PyTextureSpec),
    .tp_init = (initproc)py_texture_spec_init,
    .tp_members = pyTextureSpecMembers,
    .tp_getset = pyTextureSpecGetSet,
};
