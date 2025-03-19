#include "framebuffer.h"
#include "../debug.h"
#include "../textures/texture.h"
#include "../utility.h"
#include <structmember.h>

static const char *complete_status_to_string(GLenum status)
{
    switch (status)
    {
    case GL_FRAMEBUFFER_UNDEFINED:
        return "GL_FRAMEBUFFER_UNDEFINED";
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
    case GL_FRAMEBUFFER_UNSUPPORTED:
        return "GL_FRAMEBUFFER_UNSUPPORTED";
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
    }

    assert(0 && "Unreachable at framebuffer::complete_status_to_string");
    return NULL;
}

static bool check_framebuffer_complete(GLuint fb)
{
    GLenum completeStatus = glCheckNamedFramebufferStatus(fb, GL_FRAMEBUFFER);
    if (completeStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        PyErr_Format(PyExc_RuntimeError, "Couldn't create framebuffer: %s.", complete_status_to_string(completeStatus));
        return false;
    }

    return true;
}

// This function never fails after the framebuffer have beed initialized
static bool recreate_attachments(PyFramebuffer *self, bool isResizing)
{
    size_t nAttachments = PyList_GET_SIZE(self->specs);
    debug_log(LOG_LVL_INFO, "Creating %zu framebuffer attachments...", nAttachments);

    if (!isResizing)
        self->attachments = PyMem_Malloc(sizeof(Attachment) * nAttachments);

    for (size_t i = 0; i < nAttachments; i++)
    {
        PyAttachmentSpec *spec = (PyAttachmentSpec *)PyList_GET_ITEM(self->specs, i);
        THROW_IF(
            !is_attachment_spec((PyObject *)spec),
            PyExc_TypeError,
            "All objects in specs have to be either of FramebufferAttachment or TextureAttachment types.",
            false);

        debug_log(LOG_LVL_INFO, "Processing attachment spec %zu, of type %s...", i, Py_TYPE(spec)->tp_name);

        if (!spec->isResizable && isResizing)
        {
            // don't bother recreating attachment if it's not meant to be resized
            debug_log(LOG_LVL_INFO, "  Skipping attachment creation because the framebuffer is resizing, but attachment is not resizable.");
            continue;
        }

        // if we are not resizing or attachment is not resizable use framebuffer'e dimensions
        GLsizei width = spec->isResizable ? self->width : spec->width;
        GLsizei height = spec->isResizable ? self->height : spec->height;

        debug_log(LOG_LVL_INFO, "  Attachment info -> size: %dx%d, format: 0x%X, attachment point: 0x%X.", width, height, spec->format, spec->attachment);

        GLuint id = 0;
        if (spec->isRenderbuffer)
        {
            glCreateRenderbuffers(1, &id);
            debug_log(LOG_LVL_INFO, "  Generated attachment renderbuffer with id %d.", id);

            if (spec->samples > 1)
                glNamedRenderbufferStorageMultisample(id, spec->samples, spec->format, width, height);
            else
                glNamedRenderbufferStorage(id, spec->format, width, height);

            glNamedFramebufferRenderbuffer(self->id, spec->attachment, GL_RENDERBUFFER, id);
            debug_log(LOG_LVL_INFO, "  Attached renderbuffer attachment to attachment point 0x%X.", spec->attachment);
        }
        else
        {
            id = texture_create_fb_attachment(spec, width, height);
            debug_log(LOG_LVL_INFO, "  Generated attachment texture with id %d.", id);

            glNamedFramebufferTexture(self->id, spec->attachment, id, 0);
            debug_log(LOG_LVL_INFO, "  Attached texture attachment to attachment point 0x%X.", spec->attachment);
        }

        self->attachments[i].id = id;
        self->attachments[i].isRenderbuffer = spec->isRenderbuffer;
        self->attachments[i].target = spec->attachment;
    }

    debug_log(LOG_LVL_INFO, "Created %zu framebuffer attachments.", nAttachments);

    return true;
}

static void delete_framebuffer_contents(PyFramebuffer *self)
{
    glDeleteFramebuffers(1, &self->id);

    // check for when errors happen during initialization
    if (self->attachments == NULL)
        return;

    size_t nAttachments = PyList_GET_SIZE(self->specs);
    for (size_t i = 0; i < nAttachments; i++)
    {
        Attachment attachment = self->attachments[i];
        if (attachment.isRenderbuffer)
            glDeleteRenderbuffers(1, &attachment.id);
        else
            glDeleteTextures(1, &attachment.id);
    }
}

static PyObject *bind(PyFramebuffer *self, PyObject *Py_UNUSED(args))
{
    glBindFramebuffer(GL_FRAMEBUFFER, self->id);
    Py_RETURN_NONE;
}

static PyObject *unbind(PyFramebuffer *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Py_RETURN_NONE;
}

static PyObject *delete(PyFramebuffer *self, PyObject *Py_UNUSED(args))
{
    delete_framebuffer_contents(self);

    PyMem_Free(self->attachments);
    self->attachments = NULL;

    Py_RETURN_NONE;
}

static PyObject *resize(PyFramebuffer *self, PyObject *args)
{
    GLsizei width = 0, height = 0;
    if (!PyArg_ParseTuple(args, "ii", &width, &height))
        return NULL;

    if (width < 0 || height < 0 || (width == self->width && height == self->height))
        Py_RETURN_NONE; // do not try to create 0-sized framebuffers and skip when the size given is the same as current

    self->width = width;
    self->height = height;

    // make sure framebuffer is not bound cause it might break on some drivers
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    delete_framebuffer_contents(self);
    glCreateFramebuffers(1, &self->id);
    recreate_attachments(self, true);

    Py_RETURN_NONE;
}

static PyObject *get_attachment_id(PyFramebuffer *self, PyObject *attachment)
{
    THROW_IF(
        !PyLong_Check(attachment),
        PyExc_TypeError,
        "attachment has to be of type int.",
        NULL);

    GLenum target;
    if (!convert_attachment(attachment, &target))
        return NULL;

    size_t nAttachments = PyList_GET_SIZE(self->specs);
    for (size_t i = 0; i < nAttachments; i++)
    {
        Attachment a = self->attachments[i];
        if (a.target == target)
            return PyLong_FromUnsignedLong(a.id);
    }

    PyErr_SetString(PyExc_RuntimeError, "Couldn't find buffer attachment.");
    return NULL;
}

static PyObject *size_getter(PyFramebuffer *self, void *Py_UNUSED(closure))
{
    return PyTuple_Pack(2, PyLong_FromLong(self->width), PyLong_FromLong(self->height));
}

static int init(PyFramebuffer *self, PyObject *args, PyObject *Py_UNUSED(kwargs))
{
    if (!PyArg_ParseTuple(args, "O!ii", &PyList_Type, &self->specs, &self->width, &self->height))
        return -1;

    Py_INCREF(self->specs);

    THROW_IF(
        self->width < 0 || self->height < 0,
        PyExc_ValueError,
        "Framebuffer width and height have to be greater than 0.",
        -1);

    // catch early to prevent GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT later
    THROW_IF(
        PyList_GET_SIZE(self->specs) == 0,
        PyExc_ValueError,
        "At least one framebuffer attachment must be specified.",
        -1);

    glCreateFramebuffers(1, &self->id);
    debug_log(LOG_LVL_INFO, "Generated framebuffer id: %d.", self->id);

    if (!recreate_attachments(self, false))
        return -1;

    debug_log(LOG_LVL_INFO, "Settings up framebuffer read/draw buffers...");
    // Apply read/write specifications
    GLenum *drawBuffers = PyMem_Malloc(sizeof(GLenum) * PyList_GET_SIZE(self->specs));
    size_t drawBuffersCount = 0;
    for (size_t i = 0; i < PyList_GET_SIZE(self->specs); i++)
    {
        debug_log(LOG_LVL_INFO, "Processing attachment with index %zu...", i);

        PyAttachmentSpec *spec = (PyAttachmentSpec *)PyList_GET_ITEM(self->specs, i);
        if (is_depth_attachment(spec->attachment))
        {
            debug_log(LOG_LVL_INFO, "  Skipping attachment %zu, because it has depth/stencil format. Attachment format: 0x%x.", i, spec->format);
            continue;
        }

        if (spec->isWritable)
        {
            debug_log(LOG_LVL_INFO, "  Attachment %zu is writable. Adding draw buffer for attachment point 0x%X.", i, spec->attachment);

            drawBuffers[drawBuffersCount] = spec->attachment;
            drawBuffersCount++;
        }
    }

    if (drawBuffersCount == 0)
    {
        debug_log(LOG_LVL_INFO, "No writable attachments specified. Using GL_NONE as draw buffer.");
        glNamedFramebufferDrawBuffer(self->id, GL_NONE);
    }
    else
    {
        debug_log(LOG_LVL_INFO, "Using %zu draw buffers for the framebuffer.", drawBuffersCount);
        glNamedFramebufferDrawBuffers(self->id, drawBuffersCount, drawBuffers);
    }

    PyMem_Free(drawBuffers);

    return check_framebuffer_complete(self->id) ? 0 : -1;
}

static void dealloc(PyFramebuffer *self)
{
    delete_framebuffer_contents(self);

    if (self->attachments)
        PyMem_Free(self->attachments);

    Py_CLEAR(self->specs);
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject pyFramebufferType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_name = "pygl.framebuffer.Framebuffer",
    .tp_basicsize = sizeof(PyFramebuffer),
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)init,
    .tp_dealloc = (destructor)dealloc,
    .tp_members = (PyMemberDef[]){
        {"id", T_UINT, offsetof(PyFramebuffer, id), READONLY, NULL},
        {"width", T_INT, offsetof(PyFramebuffer, width), READONLY, NULL},
        {"height", T_INT, offsetof(PyFramebuffer, height), READONLY, NULL},
        {"specs", T_OBJECT_EX, offsetof(PyFramebuffer, specs), READONLY, NULL},
        {0},
    },
    .tp_methods = (PyMethodDef[]){
        {"bind", (PyCFunction)bind, METH_NOARGS, NULL},
        {"unbind", (PyCFunction)unbind, METH_NOARGS, NULL},
        {"delete", (PyCFunction) delete, METH_NOARGS, NULL},
        {"resize", (PyCFunction)resize, METH_VARARGS, NULL},
        {"get_attachment_id", (PyCFunction)get_attachment_id, METH_O, NULL},
        // TODO add missing framebuffer functions
        // {"clear", (PyCFunction)clear, METH_VARARGS, NULL},
        // {"read_attachment", (PyCFunction)read_attachment, METH_VARARGS, NULL},
        // {"read_pixel", (PyCFunction)read_pixel, METH_VARARGS, NULL},
        {0},
    },
    .tp_getset = (PyGetSetDef[]){
        {"size", (getter)size_getter, NULL, NULL, NULL},
        {0},
    },
};
