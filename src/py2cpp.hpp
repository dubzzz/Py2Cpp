#include <cassert>
#include <climits>
#include <stdexcept>

#include <tuple>

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
struct CppBuilder<bool>
{
  bool operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyBool_Check(pyo))
    {
      return pyo == Py_True;
    }
    throw;
  }
};

template <>
struct CppBuilder<int>
{
  int operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyLong_Check(pyo))
    {
      long v { PyLong_AsLong(pyo) };
      if (v < INT_MIN || v > INT_MAX)
      {
        throw std::overflow_error("Out of <int> boundaries");
      }
      return static_cast<int>(v);
    }
    throw;
  }
};

template <>
struct CppBuilder<unsigned int>
{
  unsigned int operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyLong_Check(pyo))
    {
      unsigned long v { PyLong_AsUnsignedLong(pyo) };
      if (v > UINT_MAX)
      {
        throw std::overflow_error("Out of <unsigned int> boundaries");
      }
      return static_cast<unsigned int>(v);
    }
    throw;
  }
};

template <>
struct CppBuilder<long>
{
  long operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyLong_Check(pyo))
    {
      return PyLong_AsLong(pyo);
    }
    throw;
  }
};

template <>
struct CppBuilder<unsigned long>
{
  unsigned long operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyLong_Check(pyo))
    {
      return PyLong_AsUnsignedLong(pyo);
    }
    throw;
  }
};

template <>
struct CppBuilder<long long>
{
  long long operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyLong_Check(pyo))
    {
      return PyLong_AsLongLong(pyo);
    }
    throw;
  }
};

template <>
struct CppBuilder<unsigned long long>
{
  unsigned long long operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyLong_Check(pyo))
    {
      return PyLong_AsUnsignedLongLong(pyo);
    }
    throw;
  }
};

template <>
struct CppBuilder<double>
{
  double operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyFloat_Check(pyo))
    {
      return PyFloat_AS_DOUBLE(pyo);
    }
    else if (PyLong_Check(pyo))
    {
      return PyLong_AsDouble(pyo);
    }
    throw;
  }
};

template <class TUPLE, std::size_t pos>
void _feedCppTuple(TUPLE& tuple, PyObject* root)
{}

template <class TUPLE, std::size_t pos, class T, class... Args>
void _feedCppTuple(TUPLE& tuple, PyObject* root)
{

  std::get<pos>(tuple) = CppBuilder<T>()(PyTuple_GetItem(root, pos));
  _feedCppTuple<TUPLE, pos +1, Args...>(tuple, root);
}


template <class... Args>
struct CppBuilder<std::tuple<Args...>>
{
  std::tuple<Args...> operator() (PyObject* pyo)
  {
    if (PyTuple_Check(pyo))
    {
      if (PyTuple_Size(pyo) == sizeof...(Args))
      {
        std::tuple<Args...> tuple { std::make_tuple(Args()...) };
        _feedCppTuple<std::tuple<Args...>, 0, Args...>(tuple, pyo);
        return tuple;
      }
    }
    throw;
  }
};

}
}

