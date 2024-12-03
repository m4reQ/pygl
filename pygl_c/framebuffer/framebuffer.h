#pragma once
#include "attachmentBase.h"
#include "../gl.h"

typedef struct
{
    GLuint id;
    bool isRenderbuffer;
    GLenum target;
} Attachment;

typedef struct
{
    PyObject_HEAD
    GL_OBJECT_HEAD;
    GLsizei width, height;
    Attachment *attachments;
    PyObject *specs;
} PyFramebuffer;

extern PyTypeObject pyFramebufferType;
