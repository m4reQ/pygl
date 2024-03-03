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
    GLsizei width, height;
    GLenum bindTarget;
} PyTexture;

extern PyTypeObject pyTextureType;

PyObject* py_texture_delete(PyTexture* self, PyObject* Py_UNUSED(args));
PyObject* py_texture_upload(PyTexture* self, PyObject* args, PyObject* kwargs);
PyObject* py_texture_bind(PyTexture* self, PyObject* Py_UNUSED(args));
PyObject* py_texture_bind_to_unit(PyTexture* self, PyObject* unit);
PyObject* py_texture_set_parameter(PyTexture* self, PyObject* args);
PyObject* py_texture_generate_mipmap(PyTexture* self, PyObject* Py_UNUSED(args));
PyObject* py_texture_bind_textures(PyObject* Py_UNUSED(cls), PyObject* args, PyObject* kwargs);
void py_texture_dealloc(PyTexture* self);

GLuint texture_create_fb_attachment(PyAttachmentSpec* spec, GLsizei width, GLsizei height);
bool texture_create(PyTexture* tex, GLenum target, PyTextureSpec* spec, bool setParameters);
void texture_set_parameter_int(GLuint tex, GLenum parameter, GLint value);
void texture_set_parameter_float(GLuint tex, GLenum parameter, GLfloat value);
