#pragma once
#include <Python.h>
#include <glad/gl.h>
#include <stdbool.h>
#include "textureSpec.h"
#include "../framebuffer/attachmentBase.h"

typedef struct
{
    PyObject_HEAD
    GLuint id;
    GLsizei width, height, layers;
    GLenum bindTarget;
} PyTexture;

extern PyMethodDef pyTextureMethods[];

void py_texture_dealloc(PyTexture* self);

GLuint texture_create_fb_attachment(PyAttachmentSpec* spec, GLsizei width, GLsizei height);
bool texture_create(PyTexture* tex, GLenum target, PyTextureSpec* spec, bool setParameters);
void texture_set_parameter_int(GLuint tex, GLenum parameter, GLint value);
void texture_set_parameter_float(GLuint tex, GLenum parameter, GLfloat value);
