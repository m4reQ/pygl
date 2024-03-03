#include "renderbufferAttachment.h"

static int init(PyAttachmentSpec* self, PyObject* args, PyObject* kwargs)
{
    static char* kwNames[] = {"width", "height", "format", "attachment", "samples", "is_resizable", NULL};

    self->isRenderbuffer = true;
    self->samples = 1;
    self->isResizable = true;

    PyObject* attachment = NULL;
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "iiIO|ip", kwNames,
        &self->width, &self->height,
        &self->format, &attachment,
        &self->samples, &self->isResizable))
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
    {"attachment", T_UINT, offsetof(PyAttachmentSpec, attachment), 0, NULL},
    {"samples", T_INT, offsetof(PyAttachmentSpec, samples), 0, NULL},
    {"is_resizable", T_BOOL, offsetof(PyAttachmentSpec, isResizable), 0, NULL},
    {0},
};

PyTypeObject pyRenderbufferAttachmentType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_name = "pygl.framebuffer.RenderbufferAttachment",
    .tp_basicsize = sizeof(PyAttachmentSpec),
    .tp_members = members,
    .tp_getset = pyAttachmentSpecGetSet,
    .tp_init = (initproc)init,
};
