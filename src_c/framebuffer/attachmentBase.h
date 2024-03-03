#pragma once
#include <stdbool.h>
#include <Python.h>
#include <structmember.h>
#include <glad/gl.h>

typedef struct
{
    PyObject_HEAD
    GLsizei width, height;
    GLenum format;
    GLenum minFilter, magFilter;
    GLenum attachment;
    GLsizei samples;
    bool isRenderbuffer;
    bool isResizable;
} PyAttachmentSpec;

extern PyGetSetDef pyAttachmentSpecGetSet[];

bool is_attachment_spec(PyObject* obj);

bool convert_attachment(PyObject* attachment, GLenum* result);
bool is_depth_attachment(GLenum attachment);
