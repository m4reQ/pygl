#include <glad/gl.h>
#include "module.h"

static PyObject *memory_barrier(PyObject *Py_UNUSED(self), PyObject *barriers)
{
    if (!PyLong_Check(barriers))
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of type pygl.rendering.Barrier, got: %s.", Py_TYPE(barriers)->tp_name);
        return NULL;
    }

    glMemoryBarrier(PyLong_AsUnsignedLong(barriers));

    Py_RETURN_NONE;
}

static PyObject *memory_barrier_by_region(PyObject *Py_UNUSED(self), PyObject *barriers)
{
    if (!PyLong_Check(barriers))
    {
        PyErr_Format(PyExc_TypeError, "Expected argument to be of type pygl.rendering.Barrier, got: %s.", Py_TYPE(barriers)->tp_name);
        return NULL;
    }

    glMemoryBarrierByRegion(PyLong_AsUnsignedLong(barriers));

    Py_RETURN_NONE;
}

static PyObject *draw_arrays(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0;
    GLint first = 0;
    GLsizei count = 0;
    if (!PyArg_ParseTuple(args, "Iii", &mode, &first, &count))
        return NULL;

    glDrawArrays(mode, first, count);

    Py_RETURN_NONE;
}

static PyObject *draw_arrays_instanced(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0;
    GLint first = 0;
    GLsizei count = 0, instanceCount = 0;
    if (!PyArg_ParseTuple(args, "Iiii", &mode, &first, &count, &instanceCount))
        return NULL;

    glDrawArraysInstanced(mode, first, count, instanceCount);

    Py_RETURN_NONE;
}

static PyObject *draw_arrays_instanced_base_instance(PyObject *Py_UNUSED(self), PyObject *args)
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

// TODO Add `offset` argument to all draw_elements_* methods
static PyObject *draw_elements(PyObject *Py_UNUSED(self), PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"mode", "count", "type", "offset", NULL};

    GLenum mode = 0, type = 0;
    GLsizei count = 0;
    void *offset = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "IiI|n", kwNames, &mode, &count, &type, &offset))
        return NULL;

    glDrawElements(mode, count, type, offset);

    Py_RETURN_NONE;
}

static PyObject *draw_elements_instanced(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0, instanceCount = 0;
    if (!PyArg_ParseTuple(args, "IiIi", &mode, &count, &type, &instanceCount))
        return NULL;

    glDrawElementsInstanced(mode, count, type, NULL, instanceCount);

    Py_RETURN_NONE;
}

static PyObject *draw_elements_instanced_base_instance(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0, instanceCount = 0;
    GLuint baseInstance = 0;
    if (!PyArg_ParseTuple(args, "IiIiI", &mode, &count, &type, &instanceCount, &baseInstance))
        return NULL;

    glDrawElementsInstancedBaseInstance(mode, count, type, NULL, instanceCount, baseInstance);

    Py_RETURN_NONE;
}

static PyObject *draw_elements_instanced_base_vertex(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0, type = 0;
    GLsizei count = 0, instanceCount = 0;
    GLint baseVertex = 0;
    if (!PyArg_ParseTuple(args, "IiIii", &mode, &count, &type, &instanceCount, &baseVertex))
        return NULL;

    glDrawElementsInstancedBaseVertex(mode, count, type, NULL, instanceCount, baseVertex);

    Py_RETURN_NONE;
}

static PyObject *draw_elements_instanced_base_vertex_base_instance(PyObject *Py_UNUSED(self), PyObject *args)
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

static PyObject *draw_arrays_indirect(PyObject *Py_UNUSED(self), PyObject *mode)
{
    if (!PyLong_Check(mode))
    {
        PyErr_SetString(PyExc_TypeError, "mode has to be of type int.");
        return NULL;
    }

    glDrawArraysIndirect(PyLong_AsUnsignedLong(mode), NULL);

    Py_RETURN_NONE;
}

static PyObject *draw_elements_indirect(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0, type = 0;
    if (!PyArg_ParseTuple(args, "II", &mode, &type))
        return NULL;

    glDrawElementsIndirect(mode, type, NULL);

    Py_RETURN_NONE;
}

static PyObject *multi_draw_arrays_indirect(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0;
    GLsizei drawCount = 0, stride = 0;
    if (!PyArg_ParseTuple(args, "Iii", &mode, &drawCount, &stride))
        return NULL;

    glMultiDrawArraysIndirect(mode, NULL, drawCount, stride);

    Py_RETURN_NONE;
}

static PyObject *multi_draw_elements_indirect(PyObject *Py_UNUSED(self), PyObject *args)
{
    GLenum mode = 0, type = 0;
    GLsizei drawCount = 0, stride = 0;
    if (!PyArg_ParseTuple(args, "IIii", &mode, &type, &drawCount, &stride))
        return NULL;

    glMultiDrawElementsIndirect(mode, type, NULL, drawCount, stride);

    Py_RETURN_NONE;
}

static PyObject *clear(PyObject *Py_UNUSED(self), PyObject *mask)
{
    if (!PyLong_Check(mask))
    {
        PyErr_SetString(PyExc_TypeError, "mask has to be of type int.");
        return NULL;
    }

    glClear(PyLong_AsUnsignedLong(mask));

    Py_RETURN_NONE;
}

static PyObject *flush(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    glFlush();

    Py_RETURN_NONE;
}

static PyObject *finish(PyObject *Py_UNUSED(self), PyObject *Py_UNUSED(args))
{
    glFinish();

    Py_RETURN_NONE;
}

static EnumDef drawModeEnum = {
    .enumName = "DrawMode",
    .values = (EnumValue[]){
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
        {0}},
};

static EnumDef elementsTypeEnum = {
    .enumName = "ElementsType",
    .values = (EnumValue[]){
        {"UNSIGNED_BYTE", GL_UNSIGNED_BYTE},
        {"UNSIGNED_SHORT", GL_UNSIGNED_SHORT},
        {"UNSIGNED_INT", GL_UNSIGNED_INT},
        {0}},
};

static EnumDef clearMaskEnum = {
    .enumName = "ClearMask",
    .values = (EnumValue[]){
        {"COLOR_BUFFER_BIT", GL_COLOR_BUFFER_BIT},
        {"DEPTH_BUFFER_BIT", GL_DEPTH_BUFFER_BIT},
        {"STENCIL_BUFFER_BIT", GL_STENCIL_BUFFER_BIT},
        {0},
    },
    .isFlag = true,
};

static EnumDef barrierEnum = {
    .enumName = "Barrier",
    .values = (EnumValue[]){
        {"VERTEX_ATTRIB_ARRAY_BARRIER_BIT", GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT},
        {"ELEMENT_ARRAY_BARRIER_BIT", GL_ELEMENT_ARRAY_BARRIER_BIT},
        {"UNIFORM_BARRIER_BIT", GL_UNIFORM_BARRIER_BIT},
        {"TEXTURE_FETCH_BARRIER_BIT", GL_TEXTURE_FETCH_BARRIER_BIT},
        {"SHADER_IMAGE_ACCESS_BARRIER_BIT", GL_SHADER_IMAGE_ACCESS_BARRIER_BIT},
        {"COMMAND_BARRIER_BIT", GL_COMMAND_BARRIER_BIT},
        {"PIXEL_BUFFER_BARRIER_BIT", GL_PIXEL_BUFFER_BARRIER_BIT},
        {"TEXTURE_UPDATE_BARRIER_BIT", GL_TEXTURE_UPDATE_BARRIER_BIT},
        {"BUFFER_UPDATE_BARRIER_BIT", GL_BUFFER_UPDATE_BARRIER_BIT},
        {"CLIENT_MAPPED_BUFFER_BARRIER_BIT", GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT},
        {"FRAMEBUFFER_BARRIER_BIT", GL_FRAMEBUFFER_BARRIER_BIT},
        {"TRANSFORM_FEEDBACK_BARRIER_BIT", GL_TRANSFORM_FEEDBACK_BARRIER_BIT},
        {"ATOMIC_COUNTER_BARRIER_BIT", GL_ATOMIC_COUNTER_BARRIER_BIT},
        {"SHADER_STORAGE_BARRIER_BIT", GL_SHADER_STORAGE_BARRIER_BIT},
        {"QUERY_BUFFER_BARRIER_BIT", GL_QUERY_BUFFER_BARRIER_BIT},
        {"ALL_BARRIER_BITS", GL_ALL_BARRIER_BITS},
        {0},
    },
};

static ModuleInfo modInfo = {
    .def = {
        PyModuleDef_HEAD_INIT,
        .m_name = "pygl.rendering",
        .m_size = -1,
        .m_methods = (PyMethodDef[]){
            {"draw_arrays", draw_arrays, METH_VARARGS, NULL},
            {"draw_arrays_instanced", draw_arrays_instanced, METH_VARARGS, NULL},
            {"draw_arrays_instanced_base_instance", draw_arrays_instanced_base_instance, METH_VARARGS, NULL},
            {"draw_elements", (PyCFunction)draw_elements, METH_VARARGS | METH_KEYWORDS, NULL},
            {"draw_elements_instanced", draw_elements_instanced, METH_VARARGS, NULL},
            {"draw_elements_instanced_base_instance", draw_elements_instanced_base_instance, METH_VARARGS, NULL},
            {"draw_elements_instanced_base_vertex", draw_elements_instanced_base_vertex, METH_VARARGS, NULL},
            {"draw_elements_instanced_base_vertex_base_instance", draw_elements_instanced_base_vertex_base_instance, METH_VARARGS, NULL},
            {"draw_arrays_indirect", draw_arrays_indirect, METH_O, NULL},
            {"draw_elements_indirect", draw_elements_indirect, METH_VARARGS, NULL},
            {"multi_draw_arrays_indirect", multi_draw_arrays_indirect, METH_VARARGS, NULL},
            {"multi_draw_elements_indirect", multi_draw_elements_indirect, METH_VARARGS, NULL},
            {"clear", clear, METH_O, NULL},
            {"memory_barrier", memory_barrier, METH_O, NULL},
            {"memory_barrier_by_region", memory_barrier_by_region, METH_O, NULL},
            {"flush", flush, METH_NOARGS, NULL},
            {"finish", finish, METH_NOARGS, NULL},
            {0},
        },
    },
    .enums = (EnumDef *[]){&drawModeEnum, &clearMaskEnum, &elementsTypeEnum, NULL},
};

PyMODINIT_FUNC PyInit_rendering()
{
    return module_create_from_info(&modInfo);
}
