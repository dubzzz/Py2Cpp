#include <iostream>

#include <Python.h>
#include "py2cpp.hpp"

using namespace dubzzz::Py2Cpp;

int main(int argc, char* argv[])
{
  {
    // PyObject*
    PyObject* pyo { Py_None };
    PyObject* out { CppBuilder<PyObject*>()(pyo) };
    std::cout << "PyObject*:\t" << out << std::endl;
  }

  {
    // double
    PyObject* pyo { PyFloat_FromDouble(1.1) };
    double v { CppBuilder<double>()(pyo) };
    std::cout << "double:\t" << v << std::endl;
  }

  return 0;
}

