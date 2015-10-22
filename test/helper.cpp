#include <Python.h>
#include "test/helper.hpp"

bool uncaught_exception()
{
  if (!! PyErr_Occurred())
  {
    PyErr_Print();
    PyErr_Clear();
    return true;
  }
  return false;
}

PyObject* get_py_dict()
{
  static PyObject* py_main { PyImport_AddModule("__main__") };
  static PyObject* py_dict { PyModule_GetDict(py_main) };
  return py_dict;
}

