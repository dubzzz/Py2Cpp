#ifndef __PY2CPP_HPP__
#define __PY2CPP_HPP__

#include <Python.h>

#include <cassert>
#include <climits>
#include <functional>
#include <memory>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include <string>

#include <type_traits>

namespace dubzzz {
namespace Py2Cpp {

/**
 * Public structures
 */

// Struct automatically decreasing PyObject counter when used with unique_ptr
//
// Syntax:
//    std::unique_ptr<PyObject, decref> python_object
struct decref
{
  void operator() (PyObject* pyo)
  {
    Py_DECREF(pyo);
  }
};

/**
 * Make object API
 * T make_cpp<T>(PyObject*)         -- build a c++ T instance given an input object PyObject
 * bool eligible_cpp<T>(PyObject*)  -- returns true if PyObject eligible to build a T
 */

// bool

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  assert(pyo);
  if (PyBool_Check(pyo))
  {
    return pyo == Py_True;
  }
  throw std::invalid_argument("Not a PyBool instance");
}

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<bool, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyBool_Check(pyo);
}

// int

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<int, typename std::decay<T>::type>::value>::type* pt = nullptr)
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

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<int, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyLong_Check(pyo) || PyInt_Check(pyo);
}

// unsigned int

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<unsigned int, typename std::decay<T>::type>::value>::type* pt = nullptr)
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
    long v { PyInt_AsLong(pyo) }; // PyInt is a long
    if (v < 0 || v > UINT_MAX)
    {
      throw std::overflow_error("Out of <unsigned int> boundaries");
    }
    return static_cast<unsigned int>(v);
  }
  throw std::invalid_argument("Not a PyLong instance");
}

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<unsigned int, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyLong_Check(pyo) || PyInt_Check(pyo);
}

// long

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<long, typename std::decay<T>::type>::value>::type* pt = nullptr)
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

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<long, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyLong_Check(pyo) || PyInt_Check(pyo);
}

// unsigned long

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<unsigned long, typename std::decay<T>::type>::value>::type* pt = nullptr)
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

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<unsigned long, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyLong_Check(pyo) || PyInt_Check(pyo);
}

// long long

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<long long, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  assert(pyo);
  if (PyLong_Check(pyo))
  {
    long long value { PyLong_AsLongLong(pyo) };
    if (!! PyErr_Occurred() && !! PyErr_ExceptionMatches(PyExc_OverflowError))
    {
      PyErr_Clear();
      throw std::overflow_error("Out of <long long> boundaries");
    }
    return value;
  }
  else if (PyInt_Check(pyo))
  {
    return static_cast<long long>(PyInt_AsLong(pyo));
  }
  throw std::invalid_argument("Not a PyLong instance");
}

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<long long, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyLong_Check(pyo) || PyInt_Check(pyo);
}

// unsigned long long

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<unsigned long long, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  assert(pyo);
  if (PyLong_Check(pyo))
  {
    unsigned long long value { PyLong_AsUnsignedLongLong(pyo) };
    if (!! PyErr_Occurred()
        && (!! PyErr_ExceptionMatches(PyExc_OverflowError) || !! PyErr_ExceptionMatches(PyExc_TypeError)))
    {
      PyErr_Clear();
      throw std::overflow_error("Out of <unsigned long long> boundaries");
    }
    return value;
  }
  else if (PyInt_Check(pyo))
  {
    long value { PyInt_AsLong(pyo) }; // PyInt is a long, LONG_MAX < ULLONG_MAX
    if (value < 0)
    {
      throw std::overflow_error("Out of <unsigned long long> boundaries");
    }
    return static_cast<unsigned long long>(value);
  }
  throw std::invalid_argument("Not a PyLong instance");
}

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<unsigned long long, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyLong_Check(pyo) || PyInt_Check(pyo);
}

// double

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<double, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  assert(pyo);
  if (PyFloat_Check(pyo))
  {
    double value { PyFloat_AsDouble(pyo) }; // PyFloat is a double
    if (value ==  INFINITY || value == -INFINITY)
    {
      PyErr_Clear();
      throw std::overflow_error("Out of <double> boundaries");
    }
    return value;
  }
  else if (PyLong_Check(pyo))
  {
    double value { PyLong_AsDouble(pyo) };
    if (!! PyErr_Occurred() && !! PyErr_ExceptionMatches(PyExc_OverflowError))
    {
      PyErr_Clear();
      throw std::overflow_error("Out of <double> boundaries");
    }
    return value;
  }
  else if (PyInt_Check(pyo))
  {
    return PyInt_AS_LONG(pyo);
  }
  throw std::invalid_argument("Neither a PyDouble nor a PyLong instance");
}

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<double, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyFloat_Check(pyo) || PyLong_Check(pyo) || PyInt_Check(pyo);
}

// std::string

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<std::string, typename std::decay<T>::type>::value>::type* pt = nullptr)
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

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<std::string, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyString_Check(pyo) || PyUnicode_Check(pyo);
}

// std::wstring

template <class T> T make_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<std::wstring, typename std::decay<T>::type>::value>::type* pt = nullptr)
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

template <class T> bool eligible_cpp(
    PyObject* pyo
    , typename std::enable_if<std::is_same<std::wstring, typename std::decay<T>::type>::value>::type* pt = nullptr)
{
  return PyString_Check(pyo) || PyUnicode_Check(pyo);
}

}
}

#endif

