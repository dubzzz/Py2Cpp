#include <Python.h>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "py2cpp.hpp"
using namespace dubzzz::Py2Cpp;

static std::ostream &operator<<(std::ostream &out, const std::vector<int> &v)
{
  out << "[";
  for (int elt : v)
  {
    out << elt << ",";
  }
  out << "]";
  return out;
}

static std::ostream &operator<<(std::ostream &out, const std::map<std::string, double> &dict)
{
  out << "{";
  for (auto &elt : dict)
  {
    out << "'" << elt.first << "'" << ":" << elt.second << ",";
  }
  out << "}";
  return out;
}

static PyObject *scalar(PyObject *self, PyObject *args)
{
  PyObject *py_v1, *py_v2;
  if (! PyArg_ParseTuple(args, "OO", &py_v1, &py_v2)) { return Py_None; }

  std::vector<int> v1 { CppBuilder<std::vector<int>>()(py_v1) };
  std::vector<int> v2 { CppBuilder<std::vector<int>>()(py_v2) };
  if (v1.size() != v2.size())
  {
    std::cout << "scalar: sizes differ" << std::endl;
    return Py_None;
  }

  std::cout << "scalar(" << v1 << "," << v2 << ")";

  decltype(v1) vres(v1.size());
  std::transform(begin(v1), end(v1), begin(v2), begin(vres), [](int e1, int e2){ return e1*e2; });
  int value = std::accumulate(begin(vres), end(vres), 0, std::plus<int>());
  
  std::cout << " = " << value << std::endl;
  return Py_None;
}

static PyObject *distance(PyObject *self, PyObject *args)
{
  PyObject *py_p1, *py_p2;
  if (! PyArg_ParseTuple(args, "OO", &py_p1, &py_p2)) { return Py_None; }

  std::map<std::string, double> p1 { CppBuilder<std::map<std::string, double>>()(py_p1) };
  std::map<std::string, double> p2 { CppBuilder<std::map<std::string, double>>()(py_p2) };
  
  double dX = p1["x"] - p2["x"];
  double dY = p1["y"] - p2["y"];
  std::cout << std::setprecision(5) << "distance(" << p1 << "," << p2
      << ") = " << std::sqrt(dX*dX + dY*dY) << std::endl;
  return Py_None;
}

static PyObject *power(PyObject *self, PyObject *args)
{
  PyObject *pyo;
  if (! PyArg_ParseTuple(args, "O", &pyo)) { return Py_None; }
  
  std::vector<std::tuple<double, int>> values;
  if (PyList_Check(pyo))
  {
    values = CppBuilder<std::vector<std::tuple<double, int>>>()(pyo);
  }
  else
  {
    std::tuple<double, int> t { CppBuilder<std::tuple<double, int>>()(pyo) };
    values.push_back(t);
  }

  for (auto &t : values)
  {
    std::cout << std::setprecision(5) << "power(" << std::get<0>(t) << "," << std::get<1>(t)
        << ") = " << std::pow(std::get<0>(t), std::get<1>(t)) << std::endl;
  }
  return Py_None;
}

static PyMethodDef ModuleMethods[] =
{
  {"scalar", scalar, METH_VARARGS, "Compute the scalar product of two Python lists"},
  {"distance", distance, METH_VARARGS, "Compute the distance between two points {'x': ::x::, 'y': ::y::}"},
  {"power", power, METH_VARARGS, "Compute the pow value of the tuple (double, exponent) or list of such tuples"},
  {NULL, NULL, 0, NULL}, //Sentinel: end of the structure
};

PyMODINIT_FUNC initto_cpp_containers()
{
  PyObject* m;
  m = Py_InitModule3("to_cpp_containers", ModuleMethods, "Example project to show how this library can be used with C++ containers");
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
  initto_cpp_containers();
}

