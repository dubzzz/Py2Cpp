# Py2Cpp [![Build Status](https://travis-ci.org/dubzzz/Py2Cpp.svg?branch=master)](https://travis-ci.org/dubzzz/Py2Cpp)[![Coverage Status](https://coveralls.io/repos/dubzzz/Py2Cpp/badge.svg?branch=master&service=github)](https://coveralls.io/github/dubzzz/Py2Cpp?branch=master)

## Metaprogramming serving Py->C++ and C++->Py

This library makes it easy to convert Python objects (Python C API) to standard C++ datatypes. It is based on a single header file that uses templates and variadic templates in order to perform the conversions. It requires C++11 or superior.

## How to use it?

### Python objects towards C++ instances

#### Basic usages: std datatypes

The file ```src/py2cpp.hpp``` defines the functor ```CppBuilder<T>```, which is responsible to build the corresponding C++ element of a given PyObject instance automatically. A same functor can be use for several PyObject.

For the moment, the conversion from Python to C++ can generate combinations of the following datatypes:
- ```PyObject*```
- ```bool```
- ```int``` and ```unsigned int```
- ```long``` and ```unsigned long```
- ```long long``` and ```unsigned long long```
- ```double```
- ```std::string``` and ```std::wstring```
- ```std::map``` -- from ```dict```
- ```std::set``` -- from ```set```
- ```std::tuple``` -- from ```tuple```
- ```std::vector``` -- from ```list```

Converting a PyObject* to a C++ element is as simple as: ```T my_cpp_elt { CppBuilder<T>()(py_object) };``` where ```T``` should be replace by the conjonction of datatypes you want.

For instance, if a PyObject* contains a ```dict(tuple(int,int,double),list(int))```, you can easily convert it into C++ datatypes by using:

```
auto out { CppBuilder<std::map<std::tuple<int,int,double>,std::vector<int>>>()(py_object) };
```

#### More advance usages: fill your own struct/class

To describe...
