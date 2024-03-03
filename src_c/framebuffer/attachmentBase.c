#include "attachmentBase.h"
#include "textureAttachment.h"
#include "renderbufferAttachment.h"

#define MAX_ATTACHMENT_OFFSET GL_COLOR_ATTACHMENT31 - GL_COLOR_ATTACHMENT0

static PyObject* is_multisampled(PyAttachmentSpec* self, void* Py_UNUSED(closure))
{
    return PyBool_FromLong(self->samples > 1);
}

static PyObject* _is_depth_attachment(PyAttachmentSpec* self, void* Py_UNUSED(closure))
{
    return PyBool_FromLong((int)is_depth_attachment(self->attachment));
}

static PyObject* size(PyAttachmentSpec* self, void* Py_UNUSED(closure))
{
    return PyTuple_Pack(2, PyLong_FromLong(self->width), PyLong_FromLong(self->height));
}

bool convert_attachment(PyObject* attachment, GLenum* result)
{
    if (PyLong_CheckExact(attachment))
    {
        int offset = PyLong_AsLong(attachment);
        if (offset < 0)
        {
            PyErr_SetString(PyExc_ValueError, "Attachment index has to be greater than 0.");
            return false;
        }

        if (offset > MAX_ATTACHMENT_OFFSET)
        {
            PyErr_Format(PyExc_ValueError, "Attachment index must be less than %d.", MAX_ATTACHMENT_OFFSET);
            return false;
        }

        *result = GL_COLOR_ATTACHMENT0 + (GLuint)offset;
    }
    else if (PyLong_Check(attachment)) // Detech Attachment enum
    {
        *result = PyLong_AsUnsignedLong(attachment);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Attachment has to be int or pygl.framebuffer.Attachment enum.");
        return false;
    }

    return true;
}

bool is_depth_attachment(GLenum attachment)
{
    return attachment == GL_DEPTH_ATTACHMENT || attachment == GL_DEPTH_STENCIL_ATTACHMENT;
}

bool is_attachment_spec(PyObject* obj)
{
    return PyObject_IsInstance(obj, (PyObject*)&pyRenderbufferAttachmentType) ||
           PyObject_IsInstance(obj, (PyObject*)&pyTextureAttachmentType);
}

PyGetSetDef pyAttachmentSpecGetSet[] = {
    {"is_multisampled", (getter)is_multisampled, NULL, NULL, NULL},
    {"is_depth_attachment", (getter)_is_depth_attachment, NULL, NULL, NULL},
    {"size", (getter)size, NULL, NULL, NULL},
    {0},
};
