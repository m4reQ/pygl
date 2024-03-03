#pragma once
#include <Python.h>
#include <glad/gl.h>
#include <stdbool.h>
#include "attachmentBase.h"

typedef struct
{
    GLuint id;
    bool isRenderbuffer;
    GLenum target;
} Attachment;

typedef struct
{
    PyObject_HEAD

    GLuint id;
    GLsizei width, height;

    Attachment* attachments;
    PyObject* specs;
} PyFramebuffer;

extern PyTypeObject pyFramebufferType;
