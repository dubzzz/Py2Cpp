#include <Python.h>
#include <iostream>

#include "py2cpp.hpp"
using namespace dubzzz::Py2Cpp;

static PyObject *read_boolean(PyObject *self, PyObject *args)
{
  PyObject *pyo;
  if (! PyArg_ParseTuple(args, "O", &pyo))
  {
    return Py_None;
  }

  std::cout << std::boolalpha << "read_boolean: " << CppBuilder<bool>()(pyo) << std::endl;
  return Py_None;
}

static PyMethodDef ModuleMethods[] =
{
  {"read_boolean", read_boolean, METH_VARARGS, "Read a boolean value coming from Python and display it"},
  {NULL, NULL, 0, NULL}, //Sentinel: end of the structure
};

PyMODINIT_FUNC initprimitives_to_cpp()
{
  PyObject* m;
  m = Py_InitModule3("primitives_to_cpp", ModuleMethods, "Example project to show how this library can be used with basic primatives");
  if (m == NULL)
    return;
}

int main(int argc, char *argv[])
{
  // Pass argv[0] to the Python interpreter
  Py_SetProgramName(argv[0]);
  
  // Initialize the Python interpreter. Required.
  Py_Initialize();

  // Add a static module
  initprimitives_to_cpp();
}

