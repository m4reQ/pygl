#pragma once
#include <Python.h>
#include "../gl.h"
#include "../framebuffer/attachmentBase.h"

typedef struct
{
    PyObject_HEAD
    GL_OBJECT_HEAD;
    GLenum target;
    GLsizei width;
    GLsizei height;
    GLsizei depth;
    GLsizei mipmaps;
} PyTexture;

extern PyTypeObject pyTextureType;

GLuint texture_create_fb_attachment(PyAttachmentSpec *spec, GLsizei width, GLsizei height);