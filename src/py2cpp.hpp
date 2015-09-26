#ifndef __PY2CPP_HPP__
#define __PY2CPP_HPP__

#include <Python.h>

#include <cassert>
#include <climits>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include <map>
#include <set>
#include <string>
#include <tuple>

namespace dubzzz {
namespace Py2Cpp {

template <class T>
struct CppBuilder;

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
      unsigned long v { PyLong_AsUnsignedLong(pyo) };
      if (!! PyErr_Occurred() && !! PyErr_ExceptionMatches(PyExc_OverflowError))
      {
        PyErr_Clear();
        throw std::overflow_error("Out of <unsigned int> boundaries");
      }
      if (v > UINT_MAX)
      {
        throw std::overflow_error("Out of <unsigned int> boundaries");
      }
      return static_cast<unsigned int>(v);
    }
    else if (PyInt_Check(pyo))
    {
      long v { PyInt_AsLong(pyo) }; // PyInt is a long, LONG_MAX < ULONG_MAX
      if (v < 0 || v > UINT_MAX)
      {
        throw std::overflow_error("Out of <unsigned int> boundaries");
      }
      return static_cast<unsigned int>(v);
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
      long value { PyLong_AsLong(pyo) };
      if (!! PyErr_Occurred() && !! PyErr_ExceptionMatches(PyExc_OverflowError))
      {
        PyErr_Clear();
        throw std::overflow_error("Out of <long> boundaries");
      }
      return value;
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
      unsigned long value { PyLong_AsUnsignedLong(pyo) };
      if (!! PyErr_Occurred() && !! PyErr_ExceptionMatches(PyExc_OverflowError))
      {
        PyErr_Clear();
        throw std::overflow_error("Out of <unsigned long> boundaries");
      }
      return value;
    }
    else if (PyInt_Check(pyo))
    {
      long value { PyInt_AsLong(pyo) }; // PyInt is a long, LONG_MAX < ULONG_MAX
      if (value < 0)
      {
        throw std::overflow_error("Out of <unsigned long> boundaries");
      }
      return static_cast<unsigned long>(value);
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
    if (PyString_Check(pyo))
    {
      long int size { PyString_Size(pyo) };
      if (size < 0)
      {
        throw std::runtime_error("Unable to retrieve C/C++ string from PyString");
      }
      return std::string(PyString_AsString(pyo), size);
    }
    else if (PyUnicode_Check(pyo))
    {
      long int size { PyUnicode_GetSize(pyo) };
      if (size < 0)
      {
        throw std::runtime_error("Unable to retrieve C/C++ string from PyUnicode");
      }
      wchar_t wstr[size];
      PyUnicode_AsWideChar(reinterpret_cast<PyUnicodeObject*>(pyo), wstr, size);
      std::wstring wide(wstr, size);
      return std::string(wide.begin(), wide.end());
    }
    throw std::invalid_argument("Neither a PyString nor a PyUnicode instance");
  }
};

template <>
struct CppBuilder<std::wstring>
{
  std::wstring operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyString_Check(pyo))
    {
      const char* str { PyString_AsString(pyo) };
      long int size { PyString_Size(pyo) };
      if (size < 0)
      {
        throw std::runtime_error("Unable to retrieve C/C++ string from PyString");
      }
      wchar_t wstr[size];
      mbstowcs(wstr, str, size);
      return std::wstring(wstr, size);
    }
    else if (PyUnicode_Check(pyo))
    {
      long int size { PyUnicode_GetSize(pyo) };
      if (size < 0)
      {
        throw std::runtime_error("Unable to retrieve C/C++ string from PyUnicode");
      }
      wchar_t wstr[size];
      PyUnicode_AsWideChar(reinterpret_cast<PyUnicodeObject*>(pyo), wstr, size);
      return std::wstring(wstr, size);
    }
    throw std::invalid_argument("Neither a PyString nor a PyUnicode instance");
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

/**
 * VECTOR has to have the following characteristics:
 * 
 * (constructor):
 *    VECTOR(size_type count)
 *    build a VECTOR of size count
 *
 * ::begin()
 * ::end()
 * ::iterator compatible with ++it
 */
template <class VECTOR>
struct CppBuilder
{
  VECTOR operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyList_Check(pyo))
    {
      unsigned int i { 0 };
      VECTOR v(PyList_Size(pyo));
      for (typename VECTOR::iterator it { v.begin() } ; it != v.end() ; ++it, ++i)
      {
        *it = CppBuilder<typename VECTOR::value_type>()(PyList_GetItem(pyo, i));
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

