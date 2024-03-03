#include "textureAttachment.h"

static int init(PyAttachmentSpec* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"width", "height", "format", "attachment", "samples", "min_filter", "mag_filter", "is_resizable", NULL};

    self->isRenderbuffer = false;
    self->isResizable = true;
    self->minFilter = GL_LINEAR;
    self->magFilter = GL_LINEAR;
    self->samples = 1;

    PyObject* attachment = NULL;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "iiIO|iIIp", kwNames,
        &self->width, &self->height,
        &self->format, &attachment,
        &self->samples,
        &self->minFilter, &self->magFilter, &self->isResizable))
        return -1;

    if (self->samples < 0)
    {
        PyErr_SetString(PyExc_ValueError, "Attachment samples count has to be greater than 0.");
        return -1;
    }

    if (!convert_attachment(attachment, &self->attachment))
        return -1;

    return 0;
}

static PyMemberDef members[] = {
    {"width", T_INT, offsetof(PyAttachmentSpec, width), 0, NULL},
    {"height", T_INT, offsetof(PyAttachmentSpec, height), 0, NULL},
    {"format", T_UINT, offsetof(PyAttachmentSpec, format), 0, NULL},
    {"min_filter", T_UINT, offsetof(PyAttachmentSpec, minFilter), 0, NULL},
    {"mag_filter", T_UINT, offsetof(PyAttachmentSpec, magFilter), 0, NULL},
    {"attachment", T_UINT, offsetof(PyAttachmentSpec, attachment), 0, NULL},
    {"samples", T_INT, offsetof(PyAttachmentSpec, samples), 0, NULL},
    {"is_resizable", T_BOOL, offsetof(PyAttachmentSpec, isResizable), 0, NULL},
    {0},
};

PyTypeObject pyTextureAttachmentType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.framebuffer.TextureAttachment",
    .tp_basicsize = sizeof(PyAttachmentSpec),
    .tp_members = members,
    .tp_getset = pyAttachmentSpecGetSet,
    .tp_init = (initproc)init,
};
