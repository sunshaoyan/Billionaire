// Copyright (C) Horizon Robotics. 2019. All Rights Reserved.
// ██╗  ██╗ █████╗  ██████╗██╗  ██╗ █████╗ ████████╗██╗  ██╗ ██████╗ ███╗   ██╗    ██████╗  ██████╗  ██╗ █████╗ 
// ██║  ██║██╔══██╗██╔════╝██║ ██╔╝██╔══██╗╚══██╔══╝██║  ██║██╔═══██╗████╗  ██║    ╚════██╗██╔═████╗███║██╔══██╗
// ███████║███████║██║     █████╔╝ ███████║   ██║   ███████║██║   ██║██╔██╗ ██║     █████╔╝██║██╔██║╚██║╚██████║
// ██╔══██║██╔══██║██║     ██╔═██╗ ██╔══██║   ██║   ██╔══██║██║   ██║██║╚██╗██║    ██╔═══╝ ████╔╝██║ ██║ ╚═══██║
// ██║  ██║██║  ██║╚██████╗██║  ██╗██║  ██║   ██║   ██║  ██║╚██████╔╝██║ ╚████║    ███████╗╚██████╔╝ ██║ █████╔╝
// ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝    ╚══════╝ ╚═════╝  ╚═╝ ╚════╝ 
// ██████╗ ██╗██╗     ██╗ ██████╗ ███╗   ██╗ █████╗ ██╗██████╗ ███████╗                                         
// ██╔══██╗██║██║     ██║██╔═══██╗████╗  ██║██╔══██╗██║██╔══██╗██╔════╝                                         
// ██████╔╝██║██║     ██║██║   ██║██╔██╗ ██║███████║██║██████╔╝█████╗                                           
// ██╔══██╗██║██║     ██║██║   ██║██║╚██╗██║██╔══██║██║██╔══██╗██╔══╝                                           
// ██████╔╝██║███████╗██║╚██████╔╝██║ ╚████║██║  ██║██║██║  ██║███████╗                                         
// ╚═════╝ ╚═╝╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝      
// Tingcheng Wu , 2019-12-16.

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "bilionaire/devcom.h"
#include "bilionaire/hbipc_errno.h"


static PyObject *
hobotx2_init_smart(PyObject *self, PyObject *args)
{
  int ret = InitSmart();
  return Py_BuildValue("i", ret);
}

static PyObject *
hobotx2_deinit_smart(PyObject *self, PyObject *args)
{
  int ret = DeinitSmart();
  return Py_BuildValue("i", ret);
}

static PyObject *
hobotx2_read_smart_frame(PyObject *self, PyObject *args)
{
  char* rptr = (char *)malloc(BUF_LEN);
  if (!rptr) {
    Log("malloc failed\n");
    return PyErr_NoMemory();
  }
  PyObject *rslt = PyTuple_New(2);
  if (!rslt) {
    Log("call PyTuple_New failed\n");
    free(rptr);
    return NULL;
  }
  int len = BUF_LEN;
  int ret = ReadSmartFrame(rptr, &len);
  if (ret) {
    Log("Read smart frame failed: %d\n", ret);
    // PyErr_SetString(PyExc_TypeError, "Read smart frame failed");
    free(rptr);
    PyTuple_SetItem(rslt, 0, Py_BuildValue("i", ret));
    PyTuple_SetItem(rslt, 1, PyBytes_FromString(""));
    return rslt;
  }

  PyObject * stream = PyBytes_FromStringAndSize(rptr, len);
  free(rptr);
  PyTuple_SetItem(rslt, 0, Py_BuildValue("i", 0));
  PyTuple_SetItem(rslt, 1, stream);
  return rslt;
}

static PyMethodDef Hobotx2Methods[] = {
    {"read_smart_frame",  hobotx2_read_smart_frame, METH_VARARGS,
          "Read for dev file."},
    {"init_smart",  hobotx2_init_smart, METH_VARARGS,
          "Initialize device for smart frame trans."},
    {"deinit_smart",  hobotx2_deinit_smart, METH_VARARGS,
          "Deinit & free resources of smart frame trans."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#ifdef PYTHON2
PyMODINIT_FUNC
inithobotx2(void)
{
  (void) Py_InitModule("hobotx2", Hobotx2Methods);
}
#else
static struct PyModuleDef hobotx2module = {
    PyModuleDef_HEAD_INIT,
    "hobotx2",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    Hobotx2Methods
};

PyMODINIT_FUNC
PyInit_hobotx2(void)
{
    return PyModule_Create(&hobotx2module);
}
#endif
