#include <Python.h>
#include "gtest/gtest.h"

#include <cfloat>
#include <climits>
#include <memory>
#include <sstream>

#include "src/py2cpp.hpp"
#include "test/helper.hpp"

using namespace dubzzz::Py2Cpp;

/** bool **/

TEST(CppBuilder_bool, True)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("True", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(true, CppBuilder<bool>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}
TEST(CppBuilder_bool, False)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("False", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(true, !CppBuilder<bool>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** int **/

TEST(CppBuilder_int, Zero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, Any)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, MinValue)
{
  std::ostringstream out; out << INT_MIN;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(INT_MIN, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, MaxValue)
{
  std::ostringstream out; out << INT_MAX;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(INT_MAX, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, LessThanMinValue)
{
  std::ostringstream out; out << INT_MIN << "-1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, MoreThanMaxValue)
{
  std::ostringstream out; out << INT_MAX << "+1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned int **/

TEST(CppBuilder_uint, Zero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<unsigned int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, Any)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<unsigned int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, MaxValue)
{
  std::ostringstream out; out << UINT_MAX;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(UINT_MAX, CppBuilder<unsigned int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, LessThanZero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, MoreThanMaxValue)
{
  std::ostringstream out; out << UINT_MAX << "+1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** long **/

TEST(CppBuilder_long, Zero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, Any)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, MinValue)
{
  std::ostringstream out; out << LONG_MIN;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LONG_MIN, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, MaxValue)
{
  std::ostringstream out; out << LONG_MAX;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LONG_MAX, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, LessThanMinValue)
{
  std::ostringstream out; out << LONG_MIN << "-1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, MoreThanMaxValue)
{
  std::ostringstream out; out << LONG_MAX << "+1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned long **/

TEST(CppBuilder_ulong, Zero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<unsigned long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, Any)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<unsigned long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, MaxValue)
{
  std::ostringstream out; out << ULONG_MAX;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(ULONG_MAX, CppBuilder<unsigned long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, LessThanZero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, FarLessThanZero)
{
  std::ostringstream out; out << LONG_MIN << "-1"; // in order to get a PyLong
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, MoreThanMaxValue)
{
  std::ostringstream out; out << ULONG_MAX << "+1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** long long **/

TEST(CppBuilder_llong, Zero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, Any)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, MinValue)
{
  std::ostringstream out; out << LLONG_MIN;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LLONG_MIN, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, MaxValue)
{
  std::ostringstream out; out << LLONG_MAX;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LLONG_MAX, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, LessThanMinValue)
{
  std::ostringstream out; out << LLONG_MIN << "-1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, MoreThanMaxValue)
{
  std::ostringstream out; out << LLONG_MAX << "+1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned long **/

TEST(CppBuilder_ullong, Zero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<unsigned long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, Any)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<unsigned long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, MaxValue)
{
  std::ostringstream out; out << ULLONG_MAX;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(ULLONG_MAX, CppBuilder<unsigned long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, LessThanZero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, FarLessThanZero)
{
  std::ostringstream out; out << LONG_MIN << "-1"; // in order to get a PyLong
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, MoreThanMaxValue)
{
  std::ostringstream out; out << ULLONG_MAX << "+1";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** double **/

TEST(CppBuilder_double, Zero)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(0, CppBuilder<double>()(pyo.get()), DBL_MIN); // PyFloat_GetMin() == DBL_MIN
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, Any)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("3.14", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(3.14, CppBuilder<double>()(pyo.get()), 2*DBL_EPSILON);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MinValue)
{
  std::ostringstream out; out << DBL_MIN;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(PyFloat_GetMin(), CppBuilder<double>()(pyo.get()), DBL_MIN);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MaxValue)
{
  std::ostringstream out; out << DBL_MAX;
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  const double tolerancy { pow(2, DBL_MAX_EXP) };
  EXPECT_NEAR(PyFloat_GetMax(), CppBuilder<double>()(pyo.get()), tolerancy);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MoreThanMaxValue)
{
  std::ostringstream out; out << DBL_MAX << "*2";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<double>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MoreThanMaxValueNegative)
{
  std::ostringstream out; out << DBL_MAX << "*(-2)";
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<double>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** std::string **/

TEST(CppBuilder_string, String)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("hello", CppBuilder<std::string>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_string, Unicode)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("u'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("hello", CppBuilder<std::string>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_string, UnicodeExotic)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(
      "u'\u15c7\u25d8\u0034\u2b15'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("\xe1\x97\x87\xe2\x97\x98\x34\xe2\xac\x95", CppBuilder<std::string>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** std::wstring **/

TEST(CppBuilder_wstring, String)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"hello", CppBuilder<std::wstring>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_wstring, Unicode)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("u'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"hello", CppBuilder<std::wstring>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_wstring, UnicodeExotic)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(
      "u'\u15c7\u25d8\u0034\u2b15'", Py_eval_input, get_py_dict(), NULL) }; //utf-16
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"\xe1\x97\x87\xe2\x97\x98\x34\xe2\xac\x95", CppBuilder<std::wstring>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** tuple **/

TEST(CppBuilder_tuple, FromTuple)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1, 'toto')", Py_eval_input, get_py_dict(), NULL) };
  std::tuple<int, std::string> expected { 1, "toto" };
  ASSERT_NE(nullptr, pyo.get());

  auto Functor =  CppBuilder<std::tuple<int, std::string>>();
  EXPECT_EQ(expected, Functor(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_tuple, FromTooSmallTuple)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1,)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());

  auto Functor =  CppBuilder<std::tuple<int, std::string>>();
  EXPECT_THROW(Functor(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_tuple, FromTooLargeTuple)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1, 'toto', 2)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());

  auto Functor =  CppBuilder<std::tuple<int, std::string>>();
  EXPECT_THROW(Functor(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

/** vector **/

TEST(CppBuilder_vector, FromList)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("[1,8,3]", Py_eval_input, get_py_dict(), NULL) };
  std::vector<int> expected { 1, 8, 3 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, CppBuilder<std::vector<int>>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_vector, FromInvalidList)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("[1,'string',3]", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<std::vector<int>>()(pyo.get()), std::invalid_argument);
  EXPECT_FALSE(uncaught_exception());
}

/** set **/

TEST(CppBuilder_set, FromSet)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("set([1,8,3])", Py_eval_input, get_py_dict(), NULL) };
  std::set<int> expected { 1, 8, 3 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, CppBuilder<std::set<int>>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}
TEST(CppBuilder_set, InputUnmodified)
{
  PyRun_SimpleString("CppBuilder_set_InputUnmodified = set([1,8,3])");
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("CppBuilder_set_InputUnmodified", Py_eval_input, get_py_dict(), NULL) };
  std::set<int> expected { 1, 8, 3 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, CppBuilder<std::set<int>>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
  
  std::unique_ptr<PyObject, decref> pyo_check { PyRun_String("CppBuilder_set_InputUnmodified == set([1,8,3])", Py_eval_input, get_py_dict(), NULL) };
  EXPECT_TRUE(CppBuilder<bool>()(pyo_check.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** map **/

TEST(CppBuilder_map, FromDict)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'x': 1, 'y': 3}", Py_eval_input, get_py_dict(), NULL) };
  std::map<std::string, int> expected { {"x", 1}, {"y", 3} };
  ASSERT_NE(nullptr, pyo.get());

  auto Functor =  CppBuilder<std::map<std::string, int>>();
  EXPECT_EQ(expected, Functor(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** struct/class **/

namespace
{
  class Point
  {
    int x, y, z;
    void setX(int x) {this->x = x;}
    void setY(int y) {this->y = y;}
    void setZ(int z) {this->z = z;}
  
  public:
    Point() : x(0), y(0), z(0) {}
    Point(int x, int y, int z) : x(x), y(y), z(z) {}
    Point(const Point&) = default;
    bool operator==(const Point& p) const {return x == p.x && y == p.y && z == p.z;}
    bool operator<(const Point& p) const {return x < p.x || (x == p.x && (y < p.y || (y == p.y && z < p.z)));}
    
    struct FromPy : CppBuilder<FromTuple<Point, int, int, int>>
    {
      FromPy() : CppBuilder<FromTuple<Point, int, int, int>>(&Point::setX, &Point::setY, &Point::setZ) {}
    };
    struct FromPyArgs : CppBuilder<FromTuple<Point, int, int, int>>
    {
      FromPyArgs() : CppBuilder<FromTuple<Point, int, int, int>>(&Point::x, &Point::y, &Point::z) {}
    };
    struct FromPyDict : CppBuilder<FromDict<Point, int, int, int>>
    {
      FromPyDict() : CppBuilder<FromDict<Point, int, int, int>>(
            make_mapping("x", &Point::setX)
            , make_mapping("y", &Point::setY)
            , make_mapping("z", &Point::setZ)) {}
    };
    struct FromPyDictArgs : CppBuilder<FromDict<Point, int, int, int>>
    {
      FromPyDictArgs() : CppBuilder<FromDict<Point, int, int, int>>(
            make_mapping("x", &Point::x)
            , make_mapping("y", &Point::y)
            , make_mapping("z", &Point::z)) {}
    };
  };
  class Line
  {
    Point pt1, pt2;
    bool oriented;
  
  public:
    Line() : pt1(), pt2(), oriented(false) {}
    Line(const Point &pt1, const Point &pt2, bool oriented) : pt1(pt1), pt2(pt2), oriented(oriented) {}
    Line(const Line&) = default;
    bool operator==(const Line& l) const {return pt1 == l.pt1 && pt2 == l.pt2 && oriented == l.oriented;}
    
    struct FromPy : CppBuilder<FromDict<Line, Point::FromPy, Point::FromPy, bool>>
    {
      FromPy() : CppBuilder<FromDict<Line, Point::FromPy, Point::FromPy, bool>>(
            make_mapping("pt1", &Line::pt1)
            , make_mapping("pt2", &Line::pt2)
            , make_mapping("oriented", &Line::oriented)) {}
    };
  };
  class Path
  {
    std::vector<Point> path;
    unsigned int length;
  
  public:
    Path() : path(), length(0) {}
    Path(const std::vector<Point> &path, unsigned int length) : path(path), length(length) {}
    Path(const Path&) = default;
    bool operator==(const Path& p) const {return path == p.path && length == p.length;}
    
    struct FromPy : CppBuilder<FromDict<Path, std::vector<Point::FromPy>, unsigned int>>
    {
      FromPy() : CppBuilder<FromDict<Path, std::vector<Point::FromPy>, unsigned int>>(
            make_mapping("path", &Path::path)
            , make_mapping("length", &Path::length)) {}
    };
  };
  class NoMove
  {
    int id;
  public:
    NoMove() : id() {};
    explicit NoMove(int id) : id(id) {}
    NoMove(NoMove const&) = delete;
    NoMove(NoMove&&) = default;
    NoMove& operator=(NoMove const&) = delete;
    NoMove& operator=(NoMove&&) = default;
    
    bool operator==(NoMove const& other) { return id == other.id; }
    bool operator<(NoMove const& other) { return id < other.id; }
    
    struct FromPy : CppBuilder<FromTuple<NoMove, int>>
    {
      FromPy() : CppBuilder<FromTuple<NoMove, int>>(&NoMove::id) {}
    };
  };
  class NoMoveOfNoMove
  {
    NoMove nmove;
  public:
    NoMoveOfNoMove() = default;
    explicit NoMoveOfNoMove(int id) : nmove(id) {}
    NoMoveOfNoMove(NoMoveOfNoMove const&) = delete;
    NoMoveOfNoMove(NoMoveOfNoMove&&) = default;
    NoMoveOfNoMove& operator=(NoMoveOfNoMove const&) = delete;
    NoMoveOfNoMove& operator=(NoMoveOfNoMove&&) = default;
    void setNoMove(NoMove&& obj) { nmove = std::move(obj); }

    bool operator==(NoMoveOfNoMove const& other) { return nmove == other.nmove; }
    
    struct FromPy : CppBuilder<FromDict<NoMoveOfNoMove, NoMove::FromPy>>
    {
      FromPy() : CppBuilder<FromDict<NoMoveOfNoMove, NoMove::FromPy>>(
            make_mapping("nmove", &NoMoveOfNoMove::nmove)) {}
    };
    struct FromPyMethod : CppBuilder<FromDict<NoMoveOfNoMove, NoMove::FromPy>>
    {
      FromPyMethod() : CppBuilder<FromDict<NoMoveOfNoMove, NoMove::FromPy>>(
            make_mapping("nmove", &NoMoveOfNoMove::setNoMove)) {}
    };
  };
}

TEST(CppBuilder_struct, FromTuple)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1, 3, 4)", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromTooSmallTuple)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1,)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(Point::FromPy()(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromTooLargeTuple)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1, 2, 3, 4)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(Point::FromPy()(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromTupleArgs)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1, 3, 4)", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyArgs()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromDict)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'y': 3, 'x': 1, 'z': 4}", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyDict()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromDictArgs)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'y': 3, 'x': 1, 'z': 4}", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyDictArgs()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromObject)
{
  PyRun_SimpleString("class Point:\n   x = 5\n   z = 14");
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("Point", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 5, 0, 14 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyDictArgs()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, VectorOfStructs)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("[(1, 3, 4), (1, 5, 5), (0, -1, 0)]", Py_eval_input, get_py_dict(), NULL) };
  Point pts[] = { { 1, 3, 4 }, { 1, 5, 5 }, { 0, -1, 0 } };
  
  ASSERT_NE(nullptr, pyo.get());
  auto ret = CppBuilder<std::vector<Point::FromPy>>()(pyo.get());
  EXPECT_EQ(3, ret.size());
  EXPECT_EQ(pts[0], ret[0]);
  EXPECT_EQ(pts[1], ret[1]);
  EXPECT_EQ(pts[2], ret[2]);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, SetOfStructs)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("set([(1, 3, 4), (1, 5, 5), (0, -1, 0)])", Py_eval_input, get_py_dict(), NULL) };
  Point pts[] = { { 1, 3, 4 }, { 1, 5, 5 }, { 0, -1, 0 } };
  
  ASSERT_NE(nullptr, pyo.get());
  auto ret = CppBuilder<std::set<Point::FromPy>>()(pyo.get());
  EXPECT_EQ(3, ret.size());
  EXPECT_NE(ret.end(), ret.find(pts[0]));
  EXPECT_NE(ret.end(), ret.find(pts[1]));
  EXPECT_NE(ret.end(), ret.find(pts[2]));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, MapOfIntStructs)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'x': (1, 0, 4), 'y': (0, 5, 9)}", Py_eval_input, get_py_dict(), NULL) };
  Point pts[] = { { 1, 0, 4 }, { 0, 5, 9 } };
  
  ASSERT_NE(nullptr, pyo.get());
  auto ret = CppBuilder<std::map<std::string, Point::FromPy>>()(pyo.get());
  EXPECT_EQ(2, ret.size());
  EXPECT_NE(ret.end(), ret.find("x"));
  EXPECT_NE(ret.end(), ret.find("y"));
  EXPECT_EQ(pts[0], ret.find("x")->second);
  EXPECT_EQ(pts[1], ret.find("y")->second);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, MapOfKeysStructAndValuesStruct)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{(0, 0, 0): (1, 0, 4), (1, 1, 2): (0, 5, 9)}", Py_eval_input, get_py_dict(), NULL) };
  Point keys[] = { { 0, 0, 0 }, { 1, 1, 2 } };
  Point pts[] = { { 1, 0, 4 }, { 0, 5, 9 } };
  
  ASSERT_NE(nullptr, pyo.get());
  auto ret = CppBuilder<std::map<Point::FromPy, Point::FromPy>>()(pyo.get());
  EXPECT_EQ(2, ret.size());
  EXPECT_NE(ret.end(), ret.find(keys[0]));
  EXPECT_NE(ret.end(), ret.find(keys[1]));
  EXPECT_EQ(pts[0], ret.find(keys[0])->second);
  EXPECT_EQ(pts[1], ret.find(keys[1])->second);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, StructOfStructs)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'oriented': True, 'pt1': (0, 0, 0), 'pt2': (1, 0, 4)}", Py_eval_input, get_py_dict(), NULL) };
  Point pt1 { 0, 0, 0 };
  Point pt2 { 1, 0, 4 };
  Line line { pt1, pt2, true };

  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(line, Line::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, StructOfComplexStructs)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'length': 56, 'path': [(0, 0, 0), (1, 0, 4), (1, 1, 2), (0, 5, 9)]}", Py_eval_input, get_py_dict(), NULL) };
  std::vector<Point> pts = { { 0, 0, 0 }, { 1, 0, 4 }, { 1, 1, 2 }, { 0, 5, 9 } };
  Path path { pts, 56 };

  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(path, Path::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** ALWAYS use move semantics when available              **/
/** some containers do not support .emplace with g++ <4.8 **/
/** following code will not compile if it not the case    **/

TEST(CppBuilder_move, ItSelf)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("(1,)", Py_eval_input, get_py_dict(), NULL) };
  NoMove expected { 1 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_TRUE(expected == NoMove::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_move, InTuple)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("((5,),)", Py_eval_input, get_py_dict(), NULL) };
  NoMove expected { 5 };
  ASSERT_NE(nullptr, pyo.get());
  auto ret = CppBuilder<std::tuple<NoMove::FromPy>>()(pyo.get());
  EXPECT_TRUE(expected == std::get<0>(ret));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_move, InVector)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("[(1,),(3,),(2,)]", Py_eval_input, get_py_dict(), NULL) };
  NoMove expected1 { 1 };
  NoMove expected2 { 3 };
  NoMove expected3 { 2 };
  ASSERT_NE(nullptr, pyo.get());
  auto ret = CppBuilder<std::vector<NoMove::FromPy>>()(pyo.get());
  EXPECT_TRUE(expected1 == ret[0]);
  EXPECT_TRUE(expected2 == ret[1]);
  EXPECT_TRUE(expected3 == ret[2]);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_move, InOtherObject)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'nmove': (2,)}", Py_eval_input, get_py_dict(), NULL) };
  NoMoveOfNoMove expected { 2 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_TRUE(expected == NoMoveOfNoMove::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_move, InOtherObject_MethodWithMove)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{'nmove': (5,)}", Py_eval_input, get_py_dict(), NULL) };
  NoMoveOfNoMove expected { 5 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_TRUE(expected == NoMoveOfNoMove::FromPyMethod()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** MISC **/

TEST(CppBuilder_mix, AnyValue)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String("{ \
        'positions':[ \
        { \
          'x': 5, \
          'y': 10, \
        }, { \
          'x': -1, \
          'y': 2, \
        }], \
      }", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  
  std::map<std::string, std::vector<std::map<std::string, int>>> elt;
  std::vector<std::map<std::string, int>> positions;
  positions.push_back({ {"x", 5}, {"y", 10} });
  positions.push_back({ {"x", -1}, {"y", 2} });
  elt["positions"] = positions;
  
  auto Functor = CppBuilder<std::map<std::string, std::vector<std::map<std::string, int>>>>();
  EXPECT_EQ(elt, Functor(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}
