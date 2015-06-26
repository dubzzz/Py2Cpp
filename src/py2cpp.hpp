#include <cassert>

#include <Python.h>

namespace dubzzz {
namespace Py2Cpp {

template <class T>
struct CppBuilder
{
  T operator() (PyObject* pyo)
  {
    assert(pyo);
    throw;
  }
};

template <>
struct CppBuilder<PyObject*>
{
  PyObject* operator() (PyObject* pyo)
  {
    assert(pyo);
    return pyo;
  }
};

template <>
struct CppBuilder<double>
{
  double operator() (PyObject* pyo)
  {
    assert(pyo);
    if (! PyFloat_Check(pyo))
    {
      throw;
    }
    return PyFloat_AS_DOUBLE(pyo);
  }
};

}
}

