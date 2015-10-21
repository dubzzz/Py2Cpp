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

class Point
{
  double x, y;

  public:
    Point() : x(), y() {}
    double distance(Point const& other) const
    {
      double dX { x - other.x };
      double dY { y - other.y };
      return sqrt(dX*dX + dY*dY);
    }
    
    struct FromPy : CppBuilder<FromDict<Point, double, double>>
    {
      FromPy() : CppBuilder<FromDict<Point, double, double>>(
            make_mapping("x", &Point::x)
            , make_mapping("y", &Point::y)) {}
    };
    friend std::ostream &operator<<(std::ostream &out, Point const& p)
    {
      return (out << "[" << p.x << "," << p.y << "]");
    }
};

class Path
{
  std::vector<Point> pts;

  public:
    Path() {}
    double length() const
    {
      double length { 0 };
      for (size_t i { 0 } ; i != pts.size() -1 ; ++i)
      {
        length += pts[i].distance(pts[i+1]);
      }
      return length;
    }

    struct FromPy : CppBuilder<FromTuple<Path, std::vector<Point::FromPy>>>
    {
      FromPy() : CppBuilder<FromTuple<Path, std::vector<Point::FromPy>>>(&Path::pts) {}
    };
    friend std::ostream &operator<<(std::ostream &out, Path const& path)
    {
      for (auto const& pt : path.pts)
      {
        out << pt;
      }
      return out;
    }
};

static PyObject *distance(PyObject *self, PyObject *args)
{
  PyObject *py_v1, *py_v2;
  if (! PyArg_ParseTuple(args, "OO", &py_v1, &py_v2)) { return Py_None; }

  Point p1 { Point::FromPy()(py_v1) };
  Point p2 { Point::FromPy()(py_v2) };

  std::cout << std::setprecision(5) << "distance(" << p1 << "," << p2 << ")" << " = " << p1.distance(p2) << std::endl;
  return Py_None;
}

static PyObject *length(PyObject *self, PyObject *args)
{
  PyObject *pyo;
  if (! PyArg_ParseTuple(args, "O", &pyo)) { return Py_None; }

  Path path { Path::FromPy()(pyo) };
  std::cout << std::setprecision(5) << "length(" << path << ")" << " = " << path.length() << std::endl;
  return Py_None;
}

static PyMethodDef ModuleMethods[] =
{
  {"distance", distance, METH_VARARGS, "Compute the distance between two points {'x': ::x::, 'y': ::y::} or objects with attributes x and y"},
  {"length", length, METH_VARARGS, "Compute the length of a path defined as a tuple containing a single list of  {'x': ::x::, 'y': ::y::} or objects with attributes x and y"},
  {NULL, NULL, 0, NULL}, //Sentinel: end of the structure
};

PyMODINIT_FUNC initto_cpp_classes()
{
  PyObject* m;
  m = Py_InitModule3("to_cpp_classes", ModuleMethods, "Example project to show how this library can be used with C++ classes");
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
  initto_cpp_classes();
}

