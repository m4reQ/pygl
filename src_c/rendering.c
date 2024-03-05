#include <glad/gl.h>
#include "enum.h"

static PyObject* draw_arrays(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0;
    GLint first = 0;
    GLsizei count = 0;
    if (!PyArg_ParseTuple(args, "Iii", &mode, &first, &count))
        return NULL;

    glDrawArrays(mode, first, count);

    Py_RETURN_NONE;
}

static PyObject* draw_arrays_instanced(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0;
    GLint first = 0;
    GLsizei count = 0, instanceCount = 0;
    if (!PyArg_ParseTuple(args, "Iiii", &mode, &first, &count, &instanceCount))
        return NULL;

    glDrawArraysInstanced(mode, first, count, instanceCount);

    Py_RETURN_NONE;
}

static PyObject* draw_arrays_instanced_base_instance(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0;
    GLint first = 0;
    GLsizei count = 0, instanceCount = 0;
    GLuint baseInstance = 0;
    if (!PyArg_ParseTuple(args, "IiiiI", &mode, &first, &count, &instanceCount, &baseInstance))
        return NULL;

    glDrawArraysInstancedBaseInstance(mode, first, count, instanceCount, baseInstance);

    Py_RETURN_NONE;
}

static PyObject* draw_elements(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0;
    if (!PyArg_ParseTuple(args, "IiI", &mode, &count, &type))
        return NULL;

    glDrawElements(mode, count, type, NULL);

    Py_RETURN_NONE;
}

static PyObject* draw_elements_instanced(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0, instanceCount = 0;
    if (!PyArg_ParseTuple(args, "IiIi", &mode, &count, &type, &instanceCount))
        return NULL;

    glDrawElementsInstanced(mode, count, type, NULL, instanceCount);

    Py_RETURN_NONE;
}

static PyObject* draw_elements_instanced_base_instance(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0, instanceCount = 0;
    GLuint baseInstance = 0;
    if (!PyArg_ParseTuple(args, "IiIiI", &mode, &count, &type, &instanceCount, &baseInstance))
        return NULL;

    glDrawElementsInstancedBaseInstance(mode, count, type, NULL, instanceCount, baseInstance);

    Py_RETURN_NONE;
}

static PyObject* draw_elements_instanced_base_vertex(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0, instanceCount = 0;
    GLint baseVertex = 0;
    if (!PyArg_ParseTuple(args, "IiIii", &mode, &count, &type, &instanceCount, &baseVertex))
        return NULL;

    glDrawElementsInstancedBaseVertex(mode, count, type, NULL, instanceCount, baseVertex);

    Py_RETURN_NONE;
}

static PyObject* draw_elements_instanced_base_vertex_base_instance(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0, instanceCount = 0;
    GLint baseVertex = 0;
    GLuint baseInstance = 0;
    if (!PyArg_ParseTuple(args, "IiIiiI", &mode, &count, &type, &instanceCount, &baseVertex, &baseInstance))
        return NULL;

    glDrawElementsInstancedBaseVertexBaseInstance(
        mode,
        count,
        type,
        NULL,
        instanceCount,
        baseVertex,
        baseInstance);

    Py_RETURN_NONE;
}

static PyObject* draw_arrays_indirect(PyObject* Py_UNUSED(self), PyObject* mode)
{
    if (!PyLong_Check(mode))
    {
        PyErr_SetString(PyExc_TypeError, "mode has to be of type int.");
        return NULL;
    }

    glDrawArraysIndirect(PyLong_AsUnsignedLong(mode), NULL);

    Py_RETURN_NONE;
}

static PyObject* draw_elements_indirect(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0, type = 0;
    if (!PyArg_ParseTuple(args, "II", &mode, &type))
        return NULL;

    glDrawElementsIndirect(mode, type, NULL);

    Py_RETURN_NONE;
}

static PyObject* multi_draw_arrays_indirect(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0;
    GLsizei drawCount = 0, stride = 0;
    if (!PyArg_ParseTuple(args, "Iii", &mode, &drawCount, &stride))
        return NULL;

    glMultiDrawArraysIndirect(mode, NULL, drawCount, stride);

    Py_RETURN_NONE;
}

static PyObject* multi_draw_elements_indirect(PyObject* Py_UNUSED(self), PyObject* args)
{
    GLenum mode = 0, type = 0;
    GLsizei drawCount = 0, stride = 0;
    if (!PyArg_ParseTuple(args, "IIii", &mode, &type, &drawCount, &stride))
        return NULL;

    glMultiDrawElementsIndirect(mode, type, NULL, drawCount, stride);

    Py_RETURN_NONE;
}

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.rendering",
    .m_size = -1,
    .m_methods = (PyMethodDef[]){
        {"draw_arrays", draw_arrays, METH_VARARGS, NULL},
        {"draw_arrays_instanced", draw_arrays_instanced, METH_VARARGS, NULL},
        {"draw_arrays_instanced_base_instance", draw_arrays_instanced_base_instance, METH_VARARGS, NULL},
        {"draw_elements", draw_elements, METH_VARARGS, NULL},
        {"draw_elements_instanced", draw_elements_instanced, METH_VARARGS, NULL},
        {"draw_elements_instanced_base_instance", draw_elements_instanced_base_instance, METH_VARARGS, NULL},
        {"draw_elements_instanced_base_vertex", draw_elements_instanced_base_vertex, METH_VARARGS, NULL},
        {"draw_elements_instanced_base_vertex_base_instance", draw_elements_instanced_base_vertex_base_instance, METH_VARARGS, NULL},
        {"draw_arrays_indirect", draw_arrays_indirect, METH_O, NULL},
        {"draw_elements_indirect", draw_elements_indirect, METH_VARARGS, NULL},
        {"multi_draw_arrays_indirect", multi_draw_arrays_indirect, METH_VARARGS, NULL},
        {"multi_draw_elements_indirect", multi_draw_elements_indirect, METH_VARARGS, NULL},
        {0}},
};

static EnumValue drawModeValues[] = {
    {"POINTS", GL_POINTS},
    {"LINE_STRIP", GL_LINE_STRIP},
    {"LINE_LOOP", GL_LINE_LOOP},
    {"LINES", GL_LINES},
    {"LINE_STRIP_ADJACENCY", GL_LINE_STRIP_ADJACENCY},
    {"LINES_ADJACENCY", GL_LINES_ADJACENCY},
    {"TRIANGLE_STRIP", GL_TRIANGLE_STRIP},
    {"TRIANGLE_FAN", GL_TRIANGLE_FAN},
    {"TRIANGLES", GL_TRIANGLES},
    {"TRIANGLE_STRIP_ADJACENCY", GL_TRIANGLE_STRIP_ADJACENCY},
    {"TRIANGLES_ADJACENCY", GL_TRIANGLES_ADJACENCY},
    {"PATCHES", GL_PATCHES},
    {0},
};
static EnumValue elementsTypeValues[] = {
    {"UNSIGNED_BYTE", GL_UNSIGNED_BYTE},
    {"UNSIGNED_SHORT", GL_UNSIGNED_SHORT},
    {"UNSIGNED_INT", GL_UNSIGNED_INT},
    {0},
};

PyMODINIT_FUNC PyInit_rendering()
{
    PyObject* mod = PyModule_Create(&moduleDef);
    if (!mod)
        return NULL;

    if (!enum_add(mod, "DrawMode", drawModeValues))
        return NULL;

    if (!enum_add(mod, "ElementsType", elementsTypeValues))
        return NULL;

    return mod;
}
