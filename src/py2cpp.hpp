#ifndef __PY2CPP_HPP__
#define __PY2CPP_HPP__

#include <cassert>
#include <climits>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include <Python.h>

namespace dubzzz {
namespace Py2Cpp {

template <class T>
struct CppBuilder
{
  T operator() (PyObject* pyo)
  {
    assert(pyo);

    std::ostringstream oss;
    oss << "No conversion implemented to convert PyObject* into " << typeid(T).name();

    throw std::invalid_argument(oss.str());
    return T {};
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
    throw std::invalid_argument("Not a PyBool instance");
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
    else if (PyInt_Check(pyo))
    {
      long v { PyInt_AS_LONG(pyo) };
      if (v < INT_MIN || v > INT_MAX)
      {
        throw std::overflow_error("Out of <int> boundaries");
      }
      return static_cast<int>(v);
    }
    throw std::invalid_argument("Not a PyLong instance");
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
      unsigned long v { PyLong_AsUnsignedLongMask(pyo) };
      if (v > UINT_MAX)
      {
        throw std::overflow_error("Out of <unsigned int> boundaries");
      }
      return static_cast<unsigned int>(v);
    }
    else if (PyInt_Check(pyo))
    {
      unsigned long v { PyInt_AsUnsignedLongMask(pyo) };
      if (v > UINT_MAX)
      {
        throw std::overflow_error("Out of <unsigned int> boundaries");
      }
      return static_cast<int>(v);
    }
    throw std::invalid_argument("Not a PyLong instance");
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
    else if (PyInt_Check(pyo))
    {
      return PyInt_AS_LONG(pyo);
    }
    throw std::invalid_argument("Not a PyLong instance");
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
    else if (PyInt_Check(pyo))
    {
      return PyInt_AsUnsignedLongMask(pyo);
    }
    throw std::invalid_argument("Not a PyLong instance");
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
    else if (PyInt_Check(pyo))
    {
      return PyInt_AS_LONG(pyo);
    }
    throw std::invalid_argument("Not a PyLong instance");
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
    else if (PyInt_Check(pyo))
    {
      return PyInt_AsUnsignedLongLongMask(pyo);
    }
    throw std::invalid_argument("Not a PyLong instance");
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
    else if (PyInt_Check(pyo))
    {
      return PyInt_AS_LONG(pyo);
    }
    throw std::invalid_argument("Neither a PyDouble nor a PyLong instance");
  }
};

template <>
struct CppBuilder<std::string>
{
  std::string operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyUnicode_Check(pyo))
    {
      /**
       * Since: Python 3.3
       * Py_ssize_t size {};
       * char* str { PyUnicode_AsUTF8AndSize(pyo, &size) };
       */
      long unsigned int size { PyUnicode_GET_DATA_SIZE(pyo) }; // depreciated in 3.3
      const char* str { PyUnicode_AS_DATA(pyo) }; // depreciated in 3.3
      return std::string(str, size);
    }
    throw std::invalid_argument("Not a PyUnicode instance");
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
    assert(pyo);
    if (PyTuple_Check(pyo))
    {
      if (PyTuple_Size(pyo) == sizeof...(Args))
      {
        std::tuple<Args...> tuple { std::make_tuple(Args()...) };
        _feedCppTuple<std::tuple<Args...>, 0, Args...>(tuple, pyo);
        return tuple;
      }
      else
      {
        std::ostringstream oss;
        oss << "PyTuple length differs from asked one: "
            << "PyTuple(" << PyTuple_Size(pyo) << ") "
            << "and std::tuple<...>(" << sizeof...(Args) << ")";
        throw std::length_error(oss.str());
      }
    }
    throw std::invalid_argument("Not a PyTuple instance");
  }
};

template <class T>
struct CppBuilder<std::vector<T>>
{
  std::vector<T> operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyList_Check(pyo))
    {
      long size { PyList_Size(pyo) };
      std::vector<T> v(size);
      for (long i { 0 } ; i != size ; ++i)
      {
        v[i] = CppBuilder<T>()(PyList_GetItem(pyo, i));
      }
      return v;
    }
    throw std::invalid_argument("Not a PyList instance");
  }
};

template <class T>
struct CppBuilder<std::set<T>>
{
  std::set<T> operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PySet_Check(pyo))
    {
      long size { PySet_Size(pyo) };
      std::vector<PyObject*> backup(size);
      std::set<T> s;
      for (long i { 0 } ; i != size ; ++i)
      {
        PyObject* popped { PySet_Pop(pyo) };
        backup[i] = popped;
        s.insert(CppBuilder<T>()(popped));
      }
      for (PyObject* popped : backup)
      {
        PySet_Add(pyo, popped);
        Py_DECREF(popped);
      }
      return s;
    }
    throw std::invalid_argument("Not a PySet instance");
  }
};

template <class K, class T>
struct CppBuilder<std::map<K,T>>
{
  std::map<K,T> operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyDict_Check(pyo))
    {
      std::map<K,T> dict;
      PyObject *key, *value;
      Py_ssize_t pos = 0;
      while (PyDict_Next(pyo, &pos, &key, &value))
      {
        dict[CppBuilder<K>()(key)] = CppBuilder<T>()(value);
      }
      return dict;
    }
    throw std::invalid_argument("Not a PyDict instance");
  }
};

}
}

#endif

