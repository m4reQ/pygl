#include "sync.h"
#include <structmember.h>
#include <glad/gl.h>
#include "utility.h"

#define AUTO_SYNC_TIMEOUT_NS 100
#define SYNC_SIGNALED(state) (state == GL_ALREADY_SIGNALED || state == GL_CONDITION_SATISFIED)

static void wait_indefinitely(GLsync sync)
{
    GLenum waitState;
    do
    {
        waitState = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, AUTO_SYNC_TIMEOUT_NS);
    } while (!SYNC_SIGNALED(waitState));
}

static PyObject *wait(PySync *self, PyObject *args, PyObject *kwargs)
{
    static char *kwNames[] = {"timeout", NULL};

    if (self->sync == NULL)
        Py_RETURN_NONE;

    GLuint64 timeout = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|K", kwNames, &timeout))
        return NULL;

    if (timeout == 0)
    {
        wait_indefinitely(self->sync);
    }
    else
    {
        GLenum waitState = glClientWaitSync(self->sync, GL_SYNC_FLUSH_COMMANDS_BIT, timeout);
        THROW_IF(
            !SYNC_SIGNALED(waitState),
            PyExc_RuntimeError,
            "Sync timeout expired.",
            NULL);
    }

    glDeleteSync(self->sync);
    self->sync = NULL;

    Py_RETURN_NONE;
}

static PyObject *set(PySync *self, PyObject *Py_UNUSED(args))
{
    self->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    Py_RETURN_NONE;
}

static PyObject *delete(PySync *self, PyObject *Py_UNUSED(args))
{
    if (self->sync)
        glDeleteSync(self->sync);

    self->sync = NULL;

    Py_RETURN_NONE;
}

static void dealloc(PySync *self)
{
    delete (self, NULL);
    Py_TYPE(self)->tp_free(self);
}

PyTypeObject pySyncType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_new = PyType_GenericNew,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_basicsize = sizeof(PySync),
    .tp_name = "pygl.sync.Sync",
    .tp_dealloc = (destructor)dealloc,
    .tp_methods = (PyMethodDef[]){
        {"set", (PyCFunction)set, METH_NOARGS, NULL},
        {"wait", (PyCFunction)wait, METH_VARARGS | METH_KEYWORDS, NULL},
        {"delete", (PyCFunction) delete, METH_NOARGS, NULL},
        {0},
    },
};

static PyModuleDef moduleDef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pygl.sync",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_sync()
{
    PyObject *mod = PyModule_Create(&moduleDef);
    if (!mod)
        return NULL;

    if (PyModule_AddType(mod, &pySyncType))
        return NULL;

    return mod;
}
