#ifndef __PY2CPP_HPP__
#define __PY2CPP_HPP__

#include <Python.h>

#include <cassert>
#include <climits>
#include <functional>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <typeinfo>

#include <map>
#include <set>
#include <string>
#include <tuple>

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

// Templated class used to help the compiler understand
// the given parameter is a builder and not a simple primative or basic structure
//
// Syntax:
//    Using<CppBuilder<T>> can be used in CppBuilder<std::map<Using<CppBuilder<T>>, int>>>
//    nb.: Using must be used when dealing with customs objects as keys of std::map
//         otherwise they are not needed but can be used
template <class FUNCTOR> struct Using : FUNCTOR {};


// FromTuple and FromDict are used to build cutsom and complex objects
// based on dicts or classes
// Syntax:
//    CppBuilder<FromTuple<MyClass, accessors' types...>>
//    CppBuilder<FromDict<MyClass, accessors' types...>>
template <class OBJ, class... Args> struct FromTuple {};
template <class OBJ, class... Args> struct FromDict {};

template <class T> struct CppBuilder;

/**
 * Identity builder
 */

template <>
struct CppBuilder<PyObject*>
{
  typedef PyObject* value_type;
  value_type operator() (PyObject* pyo)
  {
    assert(pyo);
    return pyo;
  }
};

/**
 * Primitives builders
 */

template <>
struct CppBuilder<bool>
{
  typedef bool value_type;
  value_type operator() (PyObject* pyo)
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
  typedef int value_type;
  value_type operator() (PyObject* pyo)
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
  typedef unsigned int value_type;
  value_type operator() (PyObject* pyo)
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
};

template <>
struct CppBuilder<long>
{
  typedef long value_type;
  value_type operator() (PyObject* pyo)
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
  typedef unsigned long value_type;
  value_type operator() (PyObject* pyo)
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
  typedef long long value_type;
  value_type operator() (PyObject* pyo)
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
};

template <>
struct CppBuilder<unsigned long long>
{
  typedef unsigned long long value_type;
  value_type operator() (PyObject* pyo)
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
};

template <>
struct CppBuilder<double>
{
  typedef double value_type;
  value_type operator() (PyObject* pyo)
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
};

/**
 * Strings builders
 */

template <>
struct CppBuilder<std::string>
{
  typedef std::string value_type;
  value_type operator() (PyObject* pyo)
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
  typedef std::wstring value_type;
  value_type operator() (PyObject* pyo)
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

/**
 * Tuple builder
 */

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
  typedef std::tuple<Args...> value_type;
  value_type operator() (PyObject* pyo)
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
 * Vector builder
 */

template <class FUNCTOR>
struct CppBuilder<std::vector<FUNCTOR>>
{
  typedef std::vector<typename FUNCTOR::value_type> value_type;
  value_type operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyList_Check(pyo))
    {
      unsigned int i { 0 };
      value_type v(PyList_Size(pyo));
      for (typename value_type::iterator it { v.begin() } ; it != v.end() ; ++it, ++i)
      {
        *it = FUNCTOR()(PyList_GetItem(pyo, i));
      }
      return v;
    }
    throw std::invalid_argument("Not a PyList instance");
  }
};

template <class FUNCTOR>   struct CppBuilder<std::vector<Using<FUNCTOR>>>      : CppBuilder<std::vector<FUNCTOR>> {};

template<>                 struct CppBuilder<std::vector<PyObject*>>           : CppBuilder<std::vector<CppBuilder<PyObject*>>> {};
template<>                 struct CppBuilder<std::vector<bool>>                : CppBuilder<std::vector<CppBuilder<bool>>> {};
template<>                 struct CppBuilder<std::vector<int>>                 : CppBuilder<std::vector<CppBuilder<int>>> {};
template<>                 struct CppBuilder<std::vector<unsigned int>>        : CppBuilder<std::vector<CppBuilder<unsigned int>>> {};
template<>                 struct CppBuilder<std::vector<long>>                : CppBuilder<std::vector<CppBuilder<long>>> {};
template<>                 struct CppBuilder<std::vector<unsigned long>>       : CppBuilder<std::vector<CppBuilder<unsigned long>>> {};
template<>                 struct CppBuilder<std::vector<long long>>           : CppBuilder<std::vector<CppBuilder<long long>>> {};
template<>                 struct CppBuilder<std::vector<unsigned long long>>  : CppBuilder<std::vector<CppBuilder<unsigned long long>>> {};
template<>                 struct CppBuilder<std::vector<double>>              : CppBuilder<std::vector<CppBuilder<double>>> {};
template<>                 struct CppBuilder<std::vector<std::string>>         : CppBuilder<std::vector<CppBuilder<std::string>>> {};
template<>                 struct CppBuilder<std::vector<std::wstring>>        : CppBuilder<std::vector<CppBuilder<std::wstring>>> {};
template<class... Args>    struct CppBuilder<std::vector<std::tuple<Args...>>> : CppBuilder<std::vector<CppBuilder<std::tuple<Args...>>>> {};
template<class T>          struct CppBuilder<std::vector<std::vector<T>>>      : CppBuilder<std::vector<CppBuilder<std::vector<T>>>> {};
template<class T>          struct CppBuilder<std::vector<std::set<T>>>         : CppBuilder<std::vector<CppBuilder<std::set<T>>>> {};
template<class K, class T> struct CppBuilder<std::vector<std::map<K,T>>>       : CppBuilder<std::vector<CppBuilder<std::map<K,T>>>> {};

/**
 * Set builder
 */

template <class FUNCTOR>
struct CppBuilder<std::set<FUNCTOR>>
{
  typedef std::set<typename FUNCTOR::value_type> value_type;
  value_type operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PySet_Check(pyo))
    {
      long size { PySet_Size(pyo) };
      std::vector<PyObject*> backup(size);
      value_type s;
      for (long i { 0 } ; i != size ; ++i)
      {
        PyObject* popped { PySet_Pop(pyo) };
        backup[i] = popped;
        s.insert(FUNCTOR()(popped));
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

template <class FUNCTOR>   struct CppBuilder<std::set<Using<FUNCTOR>>>      : CppBuilder<std::set<FUNCTOR>> {};

template<>                 struct CppBuilder<std::set<PyObject*>>           : CppBuilder<std::set<CppBuilder<PyObject*>>> {};
template<>                 struct CppBuilder<std::set<bool>>                : CppBuilder<std::set<CppBuilder<bool>>> {};
template<>                 struct CppBuilder<std::set<int>>                 : CppBuilder<std::set<CppBuilder<int>>> {};
template<>                 struct CppBuilder<std::set<unsigned int>>        : CppBuilder<std::set<CppBuilder<unsigned int>>> {};
template<>                 struct CppBuilder<std::set<long>>                : CppBuilder<std::set<CppBuilder<long>>> {};
template<>                 struct CppBuilder<std::set<unsigned long>>       : CppBuilder<std::set<CppBuilder<unsigned long>>> {};
template<>                 struct CppBuilder<std::set<long long>>           : CppBuilder<std::set<CppBuilder<long long>>> {};
template<>                 struct CppBuilder<std::set<unsigned long long>>  : CppBuilder<std::set<CppBuilder<unsigned long long>>> {};
template<>                 struct CppBuilder<std::set<double>>              : CppBuilder<std::set<CppBuilder<double>>> {};
template<>                 struct CppBuilder<std::set<std::string>>         : CppBuilder<std::set<CppBuilder<std::string>>> {};
template<>                 struct CppBuilder<std::set<std::wstring>>        : CppBuilder<std::set<CppBuilder<std::wstring>>> {};
template<class... Args>    struct CppBuilder<std::set<std::tuple<Args...>>> : CppBuilder<std::set<CppBuilder<std::tuple<Args...>>>> {};
template<class T>          struct CppBuilder<std::set<std::vector<T>>>      : CppBuilder<std::set<CppBuilder<std::vector<T>>>> {};
template<class T>          struct CppBuilder<std::set<std::set<T>>>         : CppBuilder<std::set<CppBuilder<std::set<T>>>> {};
template<class K, class T> struct CppBuilder<std::set<std::map<K,T>>>       : CppBuilder<std::set<CppBuilder<std::map<K,T>>>> {};

/**
 * Map builder
 */

template <class F_KEY, class F_VALUE>
struct CppBuilder<std::map<Using<F_KEY>,F_VALUE>>
{
  typedef std::map<typename F_KEY::value_type, typename F_VALUE::value_type> value_type;
  value_type operator() (PyObject* pyo)
  {
    assert(pyo);
    if (PyDict_Check(pyo))
    {
      value_type dict;
      PyObject *key, *value;
      Py_ssize_t pos = 0;
      while (PyDict_Next(pyo, &pos, &key, &value))
      {
        dict[F_KEY()(key)] = F_VALUE()(value);
      }
      return dict;
    }
    throw std::invalid_argument("Not a PyDict instance");
  }
};

template <class F_KEY, class F_VALUE> struct CppBuilder<std::map<Using<F_KEY>, Using<F_VALUE>>> : CppBuilder<std::map<Using<F_KEY>,F_VALUE>> {};
template <class K, class F_VALUE>     struct CppBuilder<std::map<K,F_VALUE>>                    : CppBuilder<std::map<Using<CppBuilder<K>>,F_VALUE>> {};

template<class U>                   struct CppBuilder<std::map<U,PyObject*>>           : CppBuilder<std::map<U,CppBuilder<PyObject*>>> {};
template<class U>                   struct CppBuilder<std::map<U,bool>>                : CppBuilder<std::map<U,CppBuilder<bool>>> {};
template<class U>                   struct CppBuilder<std::map<U,int>>                 : CppBuilder<std::map<U,CppBuilder<int>>> {};
template<class U>                   struct CppBuilder<std::map<U,unsigned int>>        : CppBuilder<std::map<U,CppBuilder<unsigned int>>> {};
template<class U>                   struct CppBuilder<std::map<U,long>>                : CppBuilder<std::map<U,CppBuilder<long>>> {};
template<class U>                   struct CppBuilder<std::map<U,unsigned long>>       : CppBuilder<std::map<U,CppBuilder<unsigned long>>> {};
template<class U>                   struct CppBuilder<std::map<U,long long>>           : CppBuilder<std::map<U,CppBuilder<long long>>> {};
template<class U>                   struct CppBuilder<std::map<U,unsigned long long>>  : CppBuilder<std::map<U,CppBuilder<unsigned long long>>> {};
template<class U>                   struct CppBuilder<std::map<U,double>>              : CppBuilder<std::map<U,CppBuilder<double>>> {};
template<class U>                   struct CppBuilder<std::map<U,std::string>>         : CppBuilder<std::map<U,CppBuilder<std::string>>> {};
template<class U>                   struct CppBuilder<std::map<U,std::wstring>>        : CppBuilder<std::map<U,CppBuilder<std::wstring>>> {};
template<class U, class... Args>    struct CppBuilder<std::map<U,std::tuple<Args...>>> : CppBuilder<std::map<U,CppBuilder<std::tuple<Args...>>>> {};
template<class U, class T>          struct CppBuilder<std::map<U,std::vector<T>>>      : CppBuilder<std::map<U,CppBuilder<std::vector<T>>>> {};
template<class U, class T>          struct CppBuilder<std::map<U,std::set<T>>>         : CppBuilder<std::map<U,CppBuilder<std::set<T>>>> {};
template<class U, class K, class T> struct CppBuilder<std::map<U,std::map<K,T>>>       : CppBuilder<std::map<U,CppBuilder<std::map<K,T>>>> {};

/**
 * Objects builders
 */

template <class OBJ, class T>
inline std::pair<std::string, std::function<void(OBJ&,T)>> make_mapping(const std::string &key, std::function<void(OBJ&,T)> fun)
{
  return std::make_pair(key, fun);
}

template <class OBJ, class T>
inline std::pair<std::string, std::function<void(OBJ&,T)>> make_mapping(const std::string &key, void (OBJ::*fun)(T))
{
  return std::make_pair(key, fun);
}

template <class OBJ, class T>
inline std::pair<std::string, std::function<void(OBJ&,T)>> make_mapping(const std::string &key, T OBJ::*member)
{
  return std::make_pair(key, [member](OBJ& obj, const T &value){ obj.*member = value; });
}

template <class OBJ, std::size_t pos, class... Args>
struct CppBuilderHelper;

template <class OBJ, std::size_t pos>
struct CppBuilderHelper<OBJ, pos>
{
  CppBuilderHelper() {}
  inline void fromDict(OBJ& obj, PyObject* pyo) const {}
  inline void fromObject(OBJ& obj, PyObject* pyo) const {}
  inline void fromTuple(OBJ& obj, PyObject* pyo) const {}
};

template <class OBJ, std::size_t pos, class T, class... Args>
struct CppBuilderHelper<OBJ,pos,T,Args...>
{
  const std::pair<std::string, std::function<void(OBJ&, T)>> callback;
  const CppBuilderHelper<OBJ, pos +1, Args...> subBuilder;
  
  // tuple's constructors
  CppBuilderHelper(std::function<void(OBJ&, T)> fun, std::function<void(OBJ&, Args)>... args)
      : callback(make_mapping("", fun)), subBuilder(args...)
  {}
  CppBuilderHelper(T OBJ::*member, Args OBJ::*... args)
      : callback(make_mapping("", member)), subBuilder(args...)
  {}
  
  // full constructors  
  CppBuilderHelper(
        std::pair<std::string, std::function<void(OBJ&, T)>> callback
        , std::pair<std::string, std::function<void(OBJ&, Args)>>... args)
      : callback(callback), subBuilder(args...)
  {}
  
  inline void fromDict(OBJ& obj, PyObject* pyo) const
  {
    PyObject *pyo_item { PyDict_GetItemString(pyo, callback.first.c_str()) };
    if (pyo_item)
    {
      T value { CppBuilder<T>()(pyo_item) };
      callback.second(obj, value);
    }
    subBuilder.fromDict(obj, pyo);
  }
  
  inline void fromObject(OBJ& obj, PyObject* pyo) const
  {
    if (PyObject_HasAttrString(pyo, callback.first.c_str()))
    {
      std::unique_ptr<PyObject, decref> pyo_item { PyObject_GetAttrString(pyo, callback.first.c_str()) };
      T value { CppBuilder<T>()(pyo_item.get()) };
      callback.second(obj, value);
    }
    subBuilder.fromObject(obj, pyo);
  }
  
  inline void fromTuple(OBJ& obj, PyObject* pyo) const
  {
    T value { CppBuilder<T>()(PyTuple_GetItem(pyo, pos)) };
    callback.second(obj, value);
    subBuilder.fromTuple(obj, pyo);
  }
};

template <class OBJ, class... Args>
struct CppBuilder<FromTuple<OBJ, Args...>>
{
  typedef OBJ value_type;
  const CppBuilderHelper<OBJ, 0, Args...> subBuilder;
  
  CppBuilder(std::function<void(OBJ&, Args)>... args)
      : subBuilder(args...)
  {}
  
  CppBuilder(Args OBJ::*... args)
      : subBuilder(args...)
  {}
  
  value_type operator() (PyObject* pyo)
  {
    assert(pyo);
    
    if (PyTuple_Check(pyo))
    {
      if (PyTuple_Size(pyo) == sizeof...(Args))
      {
        OBJ obj;
        subBuilder.fromTuple(obj, pyo);
        return obj;
      }
      else
      {
        std::ostringstream oss;
        oss << "PyTuple length differs from asked one: "
            << "PyTuple(" << PyTuple_Size(pyo) << ") "
            << "and FromTuple<...>(" << sizeof...(Args) << ")";
        throw std::length_error(oss.str());
      }
    }
    throw std::invalid_argument("Not a PyTuple instance");
  }
};

template <class OBJ, class... Args>
struct CppBuilder<FromDict<OBJ, Args...>>
{
  typedef OBJ value_type;
  CppBuilderHelper<OBJ, 0, Args...> subBuilder;
  
  CppBuilder(std::pair<std::string, std::function<void(OBJ&, Args)>>... args)
      : subBuilder(args...)
  {}
  
  value_type operator() (PyObject* pyo)
  {
    assert(pyo);
    
    OBJ obj;
    if (PyDict_Check(pyo))
    {
      subBuilder.fromDict(obj, pyo);
    }
    else
    {
      subBuilder.fromObject(obj, pyo);
    }
    return obj;
  }
};

}
}

#endif

