#ifndef __HELPER_HPP__
#define __HELPER_HPP__

#include <Python.h>

// true if a Python error occurred and was not catch
// Print the error and free error space
bool uncaught_exception();

// PyObject corresponding to Python environment
PyObject* get_py_dict();

#endif

