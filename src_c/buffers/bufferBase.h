#pragma once
#include <Python.h>
#include <structmember.h>
#include <glad/gl.h>
#include <stdbool.h>

typedef struct
{
    PyObject_HEAD
    GLuint id;
    GLsizeiptr size;
    GLintptr currentOffset;
    void* dataPtr;
    GLenum flags;
} PyBufferBase;

bool is_buffer(PyObject* obj);

// helper functions
GLuint create_gl_buffer(GLsizeiptr size, GLenum flags);
void delete_gl_buffer(GLuint buf);
void* map_gl_buffer(GLuint buf, GLintptr offset, GLsizeiptr size, GLenum flags);
void unmap_gl_buffer(GLuint buf);
bool store_to_buffer(PyBufferBase* buf, PyObject* data);

// common python structures
extern PyMemberDef pyBufferBaseMembers[];
