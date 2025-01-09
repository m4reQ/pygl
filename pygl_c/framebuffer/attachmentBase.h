#pragma once
#include <stdbool.h>
#include <Python.h>
#include <structmember.h>
#include <glad/gl.h>

// DONT CHANGE LAYOUT OF THIS STRUCT
// specifically location of bool fields
typedef struct
{
    PyObject_HEAD
    bool isRenderbuffer;
    bool isResizable;
    bool isWritable;
    bool isReadable;
    GLsizei width, height;
    GLenum format;
    GLenum minFilter, magFilter;
    GLenum attachment;
    GLsizei samples;
} PyAttachmentSpec;

extern PyGetSetDef pyAttachmentSpecGetSet[];

bool is_attachment_spec(PyObject* obj);

bool convert_attachment(PyObject* attachment, GLenum* result);
bool is_depth_attachment(GLenum attachment);
