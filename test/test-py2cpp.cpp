#include <Python.h>
#include "gtest/gtest.h"

#include <cfloat>
#include <climits>
#include <memory>
#include <sstream>

#include "src/py2cpp.hpp"
#include "test/helper.hpp"

using namespace dubzzz::Py2Cpp;

namespace
{
  /**
   * Class responsible to check whether or not
   * PyObject's ref count was impacted by the run
   */
  struct unique_ptr_ctn : std::unique_ptr<PyObject, decref>
  {
    Py_ssize_t initial;
    std::vector<Py_ssize_t> subObjectsCtn;
    
    std::vector<Py_ssize_t> get_sub_objects_ctn()
    {
      PyObject* pyo { get() };
      std::vector<Py_ssize_t> ctns;
      
      if (PyTuple_Check(pyo))
      {
        for (Py_ssize_t i { 0 } ; i != PyTuple_Size(pyo) ; ++i)
        {
          ctns.push_back(Py_REFCNT(PyTuple_GetItem(pyo, i)));
        }
      }
      else if (PyList_Check(pyo))
      {
        for (Py_ssize_t i { 0 } ; i != PyList_Size(pyo) ; ++i)
        {
          ctns.push_back(Py_REFCNT(PyList_GetItem(pyo, i)));
        }
      }
      else if (PySet_Check(pyo))
      {
        long size { PySet_Size(pyo) };
        PyObject* backup[size];
        for (auto& elt : backup)
        {
          PyObject* popped { PySet_Pop(pyo) };
          elt = popped;
          ctns.push_back(Py_REFCNT(popped));
        }
        for (auto& popped : backup)
        {
          PySet_Add(pyo, popped);
          Py_DECREF(popped);
        }
      }
      else if (PyDict_Check(pyo))
      {
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(pyo, &pos, &key, &value))
        {
          ctns.push_back(Py_REFCNT(key));
          ctns.push_back(Py_REFCNT(value));
        }
      }
      return ctns;
    }
    
    explicit unique_ptr_ctn(PyObject* pyo) : std::unique_ptr<PyObject, decref>(pyo)
    {
      initial = Py_REFCNT(pyo);
      subObjectsCtn = get_sub_objects_ctn();
    }

    ~unique_ptr_ctn()
    {
      EXPECT_EQ(initial, Py_REFCNT(get()));
      EXPECT_EQ(subObjectsCtn, get_sub_objects_ctn()) << "The ref count of sub items has changed";
    }
  };
}

/** bool **/

TEST(CppBuilder_bool, True)
{
  unique_ptr_ctn pyo { PyRun_String("True", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(true, CppBuilder<bool>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}
TEST(CppBuilder_bool, False)
{
  unique_ptr_ctn pyo { PyRun_String("False", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(true, !CppBuilder<bool>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** int **/

TEST(CppBuilder_int, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, MinValue)
{
  std::ostringstream out; out << INT_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(INT_MIN, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, MaxValue)
{
  std::ostringstream out; out << INT_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(INT_MAX, CppBuilder<int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, LessThanMinValue)
{
  std::ostringstream out; out << INT_MIN << "-1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_int, MoreThanMaxValue)
{
  std::ostringstream out; out << INT_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned int **/

TEST(CppBuilder_uint, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<unsigned int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<unsigned int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, MaxValue)
{
  std::ostringstream out; out << UINT_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(UINT_MAX, CppBuilder<unsigned int>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, LessThanZero)
{
  unique_ptr_ctn pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_uint, MoreThanMaxValue)
{
  std::ostringstream out; out << UINT_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned int>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** long **/

TEST(CppBuilder_long, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, MinValue)
{
  std::ostringstream out; out << LONG_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LONG_MIN, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, MaxValue)
{
  std::ostringstream out; out << LONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LONG_MAX, CppBuilder<long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, LessThanMinValue)
{
  std::ostringstream out; out << LONG_MIN << "-1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_long, MoreThanMaxValue)
{
  std::ostringstream out; out << LONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned long **/

TEST(CppBuilder_ulong, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<unsigned long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<unsigned long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, MaxValue)
{
  std::ostringstream out; out << ULONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(ULONG_MAX, CppBuilder<unsigned long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, LessThanZero)
{
  unique_ptr_ctn pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, FarLessThanZero)
{
  std::ostringstream out; out << LONG_MIN << "-1"; // in order to get a PyLong
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ulong, MoreThanMaxValue)
{
  std::ostringstream out; out << ULONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** long long **/

TEST(CppBuilder_llong, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, MinValue)
{
  std::ostringstream out; out << LLONG_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LLONG_MIN, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, MaxValue)
{
  std::ostringstream out; out << LLONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LLONG_MAX, CppBuilder<long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, LessThanMinValue)
{
  std::ostringstream out; out << LLONG_MIN << "-1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_llong, MoreThanMaxValue)
{
  std::ostringstream out; out << LLONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned long **/

TEST(CppBuilder_ullong, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, CppBuilder<unsigned long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, CppBuilder<unsigned long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, MaxValue)
{
  std::ostringstream out; out << ULLONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(ULLONG_MAX, CppBuilder<unsigned long long>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, LessThanZero)
{
  unique_ptr_ctn pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, FarLessThanZero)
{
  std::ostringstream out; out << LONG_MIN << "-1"; // in order to get a PyLong
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_ullong, MoreThanMaxValue)
{
  std::ostringstream out; out << ULLONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<unsigned long long>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** double **/

TEST(CppBuilder_double, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(0, CppBuilder<double>()(pyo.get()), DBL_MIN); // PyFloat_GetMin() == DBL_MIN
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, Any)
{
  unique_ptr_ctn pyo { PyRun_String("3.14", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(3.14, CppBuilder<double>()(pyo.get()), 2*DBL_EPSILON);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MinValue)
{
  std::ostringstream out; out << DBL_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(PyFloat_GetMin(), CppBuilder<double>()(pyo.get()), DBL_MIN);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MaxValue)
{
  std::ostringstream out; out << DBL_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  const double tolerancy { pow(2, DBL_MAX_EXP) };
  EXPECT_NEAR(PyFloat_GetMax(), CppBuilder<double>()(pyo.get()), tolerancy);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MoreThanMaxValue)
{
  std::ostringstream out; out << DBL_MAX << "*2";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<double>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_double, MoreThanMaxValueNegative)
{
  std::ostringstream out; out << DBL_MAX << "*(-2)";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<double>()(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** std::string **/

TEST(CppBuilder_string, String)
{
  unique_ptr_ctn pyo { PyRun_String("'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("hello", CppBuilder<std::string>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_string, Unicode)
{
  unique_ptr_ctn pyo { PyRun_String("u'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("hello", CppBuilder<std::string>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_string, UnicodeExotic)
{
  unique_ptr_ctn pyo { PyRun_String(
      "u'\u15c7\u25d8\u0034\u2b15'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("\xe1\x97\x87\xe2\x97\x98\x34\xe2\xac\x95", CppBuilder<std::string>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** std::wstring **/

TEST(CppBuilder_wstring, String)
{
  unique_ptr_ctn pyo { PyRun_String("'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"hello", CppBuilder<std::wstring>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_wstring, Unicode)
{
  unique_ptr_ctn pyo { PyRun_String("u'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"hello", CppBuilder<std::wstring>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_wstring, UnicodeExotic)
{
  unique_ptr_ctn pyo { PyRun_String(
      "u'\u15c7\u25d8\u0034\u2b15'", Py_eval_input, get_py_dict(), NULL) }; //utf-16
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"\xe1\x97\x87\xe2\x97\x98\x34\xe2\xac\x95", CppBuilder<std::wstring>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** tuple **/

TEST(CppBuilder_tuple, FromTuple)
{
  unique_ptr_ctn pyo { PyRun_String("(1, 'toto')", Py_eval_input, get_py_dict(), NULL) };
  std::tuple<int, std::string> expected { 1, "toto" };
  ASSERT_NE(nullptr, pyo.get());

  auto Functor =  CppBuilder<std::tuple<int, std::string>>();
  EXPECT_EQ(expected, Functor(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_tuple, FromTooSmallTuple)
{
  unique_ptr_ctn pyo { PyRun_String("(1,)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());

  auto Functor =  CppBuilder<std::tuple<int, std::string>>();
  EXPECT_THROW(Functor(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_tuple, FromTooLargeTuple)
{
  unique_ptr_ctn pyo { PyRun_String("(1, 'toto', 2)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());

  auto Functor =  CppBuilder<std::tuple<int, std::string>>();
  EXPECT_THROW(Functor(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

/** vector **/

TEST(CppBuilder_vector, FromList)
{
  unique_ptr_ctn pyo { PyRun_String("[1,8,3]", Py_eval_input, get_py_dict(), NULL) };
  std::vector<int> expected { 1, 8, 3 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, CppBuilder<std::vector<int>>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_vector, FromInvalidList)
{
  unique_ptr_ctn pyo { PyRun_String("[1,'string',3]", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(CppBuilder<std::vector<int>>()(pyo.get()), std::invalid_argument);
  EXPECT_FALSE(uncaught_exception());
}

/** set **/

TEST(CppBuilder_set, FromSet)
{
  unique_ptr_ctn pyo { PyRun_String("set([1,8,3])", Py_eval_input, get_py_dict(), NULL) };
  std::set<int> expected { 1, 8, 3 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, CppBuilder<std::set<int>>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}
TEST(CppBuilder_set, InputUnmodified)
{
  PyRun_SimpleString("CppBuilder_set_InputUnmodified = set([1,8,3])");
  unique_ptr_ctn pyo { PyRun_String("CppBuilder_set_InputUnmodified", Py_eval_input, get_py_dict(), NULL) };
  std::set<int> expected { 1, 8, 3 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, CppBuilder<std::set<int>>()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
  
  unique_ptr_ctn pyo_check { PyRun_String("CppBuilder_set_InputUnmodified == set([1,8,3])", Py_eval_input, get_py_dict(), NULL) };
  EXPECT_TRUE(CppBuilder<bool>()(pyo_check.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** map **/

TEST(CppBuilder_map, FromDict)
{
  unique_ptr_ctn pyo { PyRun_String("{'x': 1, 'y': 3}", Py_eval_input, get_py_dict(), NULL) };
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
  unique_ptr_ctn pyo { PyRun_String("(1, 3, 4)", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromTooSmallTuple)
{
  unique_ptr_ctn pyo { PyRun_String("(1,)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(Point::FromPy()(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromTooLargeTuple)
{
  unique_ptr_ctn pyo { PyRun_String("(1, 2, 3, 4)", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(Point::FromPy()(pyo.get()), std::length_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromTupleArgs)
{
  unique_ptr_ctn pyo { PyRun_String("(1, 3, 4)", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyArgs()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromDict)
{
  unique_ptr_ctn pyo { PyRun_String("{'y': 3, 'x': 1, 'z': 4}", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyDict()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromDictArgs)
{
  unique_ptr_ctn pyo { PyRun_String("{'y': 3, 'x': 1, 'z': 4}", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 1, 3, 4 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyDictArgs()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, FromObject)
{
  PyRun_SimpleString("class Point:\n   x = 5\n   z = 14");
  unique_ptr_ctn pyo { PyRun_String("Point", Py_eval_input, get_py_dict(), NULL) };
  Point expected { 5, 0, 14 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(expected, Point::FromPyDictArgs()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, VectorOfStructs)
{
  unique_ptr_ctn pyo { PyRun_String("[(1, 3, 4), (1, 5, 5), (0, -1, 0)]", Py_eval_input, get_py_dict(), NULL) };
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
  unique_ptr_ctn pyo { PyRun_String("set([(1, 3, 4), (1, 5, 5), (0, -1, 0)])", Py_eval_input, get_py_dict(), NULL) };
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
  unique_ptr_ctn pyo { PyRun_String("{'x': (1, 0, 4), 'y': (0, 5, 9)}", Py_eval_input, get_py_dict(), NULL) };
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
  unique_ptr_ctn pyo { PyRun_String("{(0, 0, 0): (1, 0, 4), (1, 1, 2): (0, 5, 9)}", Py_eval_input, get_py_dict(), NULL) };
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
  unique_ptr_ctn pyo { PyRun_String("{'oriented': True, 'pt1': (0, 0, 0), 'pt2': (1, 0, 4)}", Py_eval_input, get_py_dict(), NULL) };
  Point pt1 { 0, 0, 0 };
  Point pt2 { 1, 0, 4 };
  Line line { pt1, pt2, true };

  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(line, Line::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_struct, StructOfComplexStructs)
{
  unique_ptr_ctn pyo { PyRun_String("{'length': 56, 'path': [(0, 0, 0), (1, 0, 4), (1, 1, 2), (0, 5, 9)]}", Py_eval_input, get_py_dict(), NULL) };
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
  unique_ptr_ctn pyo { PyRun_String("(1,)", Py_eval_input, get_py_dict(), NULL) };
  NoMove expected { 1 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_TRUE(expected == NoMove::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_move, InTuple)
{
  unique_ptr_ctn pyo { PyRun_String("((5,),)", Py_eval_input, get_py_dict(), NULL) };
  NoMove expected { 5 };
  ASSERT_NE(nullptr, pyo.get());
  auto ret = CppBuilder<std::tuple<NoMove::FromPy>>()(pyo.get());
  EXPECT_TRUE(expected == std::get<0>(ret));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_move, InVector)
{
  unique_ptr_ctn pyo { PyRun_String("[(1,),(3,),(2,)]", Py_eval_input, get_py_dict(), NULL) };
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
  unique_ptr_ctn pyo { PyRun_String("{'nmove': (2,)}", Py_eval_input, get_py_dict(), NULL) };
  NoMoveOfNoMove expected { 2 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_TRUE(expected == NoMoveOfNoMove::FromPy()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(CppBuilder_move, InOtherObject_MethodWithMove)
{
  unique_ptr_ctn pyo { PyRun_String("{'nmove': (5,)}", Py_eval_input, get_py_dict(), NULL) };
  NoMoveOfNoMove expected { 5 };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_TRUE(expected == NoMoveOfNoMove::FromPyMethod()(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** MISC **/

TEST(CppBuilder_mix, AnyValue)
{
  unique_ptr_ctn pyo { PyRun_String("{ \
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

/** CppBuilder<T>::eligible **/

static std::string maxLL()
{
  std::ostringstream out;
  out << LLONG_MAX;
  return out.str();
}

template <class BUILDER>
static void filterThrow(BUILDER const& builder, PyObject* pyo)
{
  try
  {
    builder(pyo);
  }
  catch(std::invalid_argument const&)
  {
    throw;
  }
  catch(...) {}
}

template <class BUILDER>
static void shouldBeEligible(BUILDER const& builder, std::string const& pyQuery)
{
  unique_ptr_ctn pyo { PyRun_String(pyQuery.c_str(), Py_eval_input, get_py_dict(), NULL) };
  EXPECT_TRUE(builder.eligible(pyo.get())) << "Conversion should be possible for: '" << pyQuery << "'";
  EXPECT_NO_THROW(filterThrow(builder, pyo.get())) << "Conversion should be possible for: '" << pyQuery << "'";;
}
template <class BUILDER>
static void shouldNotBeEligible(BUILDER const& builder, std::string const& pyQuery)
{
  unique_ptr_ctn pyo { PyRun_String(pyQuery.c_str(), Py_eval_input, get_py_dict(), NULL) };
  EXPECT_FALSE(builder.eligible(pyo.get())) << "Conversion should not be possible for: '" << pyQuery << "'";
  EXPECT_THROW(builder(pyo.get()), std::invalid_argument) << "Conversion should not be possible for: '" << pyQuery << "'";
}

TEST(CppBuilder_eligible, bool)
{
  auto builder = CppBuilder<bool>();

  shouldBeEligible(builder, "True");
  
  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "1");
  shouldNotBeEligible(builder, maxLL());
  shouldNotBeEligible(builder, "1.2");
  shouldNotBeEligible(builder, "'This is a string'");
  shouldNotBeEligible(builder, "u'This is a unicode string'");
  shouldNotBeEligible(builder, "(1,2,3)");
  shouldNotBeEligible(builder, "[1,2,3]");
  shouldNotBeEligible(builder, "set([1,2,3])");
  shouldNotBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");
}

template <class TYPE>
static void testInteger()
{
  auto builder = CppBuilder<TYPE>();

  shouldBeEligible(builder, "True");
  shouldBeEligible(builder, "1");
  shouldBeEligible(builder, maxLL());
  
  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "1.2");
  shouldNotBeEligible(builder, "'This is a string'");
  shouldNotBeEligible(builder, "u'This is a unicode string'");
  shouldNotBeEligible(builder, "(1,2,3)");
  shouldNotBeEligible(builder, "[1,2,3]");
  shouldNotBeEligible(builder, "set([1,2,3])");
  shouldNotBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");
}

TEST(CppBuilder_eligible, int)
{
  testInteger<int>();
}
TEST(CppBuilder_eligible, unsigned_int)
{
  testInteger<unsigned int>();
}

TEST(CppBuilder_eligible, long)
{
  testInteger<long>();
}
TEST(CppBuilder_eligible, unsigned_long)
{
  testInteger<unsigned long>();
}

TEST(CppBuilder_eligible, long_long)
{
  testInteger<long long>();
}
TEST(CppBuilder_eligible, unsigned_long_long)
{
  testInteger<unsigned long long>();
}

TEST(CppBuilder_eligible, double)
{
  auto builder = CppBuilder<double>();

  shouldBeEligible(builder, "True");
  shouldBeEligible(builder, "1");
  shouldBeEligible(builder, maxLL());
  shouldBeEligible(builder, "1.2");
  
  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "'This is a string'");
  shouldNotBeEligible(builder, "u'This is a unicode string'");
  shouldNotBeEligible(builder, "(1,2,3)");
  shouldNotBeEligible(builder, "[1,2,3]");
  shouldNotBeEligible(builder, "set([1,2,3])");
  shouldNotBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");
}

template <class TYPE>
static void testString()
{
  auto builder = CppBuilder<TYPE>();

  shouldBeEligible(builder, "'This is a string'");
  shouldBeEligible(builder, "u'This is a unicode string'");
  
  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "True");
  shouldNotBeEligible(builder, "1");
  shouldNotBeEligible(builder, maxLL());
  shouldNotBeEligible(builder, "1.2");
  shouldNotBeEligible(builder, "(1,2,3)");
  shouldNotBeEligible(builder, "[1,2,3]");
  shouldNotBeEligible(builder, "set([1,2,3])");
  shouldNotBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");
}

TEST(CppBuilder_eligible, string)
{
  testString<std::string>();
}
TEST(CppBuilder_eligible, wstring)
{
  testString<std::wstring>();
}

TEST(CppBuilder_eligible, tuple)
{
  auto builder = CppBuilder<std::tuple<int, int, int>>();
  
  shouldBeEligible(builder, "(1,2,3)");

  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "True");
  shouldNotBeEligible(builder, "1");
  shouldNotBeEligible(builder, maxLL());
  shouldNotBeEligible(builder, "1.2");
  shouldNotBeEligible(builder, "'This is a string'");
  shouldNotBeEligible(builder, "u'This is a unicode string'");
  shouldNotBeEligible(builder, "[1,2,3]");
  shouldNotBeEligible(builder, "(1,2)");
  shouldNotBeEligible(builder, "(1,'string',3)");
  shouldNotBeEligible(builder, "set([1,2,3])");
  shouldNotBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");
}

TEST(CppBuilder_eligible, vector)
{
  auto builder = CppBuilder<std::vector<int>>();
  
  shouldBeEligible(builder, "[]");
  shouldBeEligible(builder, "[1,2,3]");

  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "True");
  shouldNotBeEligible(builder, "1");
  shouldNotBeEligible(builder, maxLL());
  shouldNotBeEligible(builder, "1.2");
  shouldNotBeEligible(builder, "'This is a string'");
  shouldNotBeEligible(builder, "u'This is a unicode string'");
  shouldNotBeEligible(builder, "(1,2,3)");
  shouldNotBeEligible(builder, "['string']");
  shouldNotBeEligible(builder, "set([1,2,3])");
  shouldNotBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");
}

TEST(CppBuilder_eligible, set)
{
  auto builder = CppBuilder<std::set<int>>();
  
  shouldBeEligible(builder, "set([])");
  shouldBeEligible(builder, "set([1,2,3])");

  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "True");
  shouldNotBeEligible(builder, "1");
  shouldNotBeEligible(builder, maxLL());
  shouldNotBeEligible(builder, "1.2");
  shouldNotBeEligible(builder, "'This is a string'");
  shouldNotBeEligible(builder, "u'This is a unicode string'");
  shouldNotBeEligible(builder, "(1,2,3)");
  shouldNotBeEligible(builder, "[1,2,3]");
  shouldNotBeEligible(builder, "set(['string'])");
  shouldNotBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");
}

TEST(CppBuilder_eligible, map)
{
  auto builder = CppBuilder<std::map<std::string, int>>();
  
  shouldBeEligible(builder, "{}");
  shouldBeEligible(builder, "{'x': 1, 'y': 2, 'z': 3}");

  shouldNotBeEligible(builder, "None");
  shouldNotBeEligible(builder, "True");
  shouldNotBeEligible(builder, "1");
  shouldNotBeEligible(builder, maxLL());
  shouldNotBeEligible(builder, "1.2");
  shouldNotBeEligible(builder, "'This is a string'");
  shouldNotBeEligible(builder, "u'This is a unicode string'");
  shouldNotBeEligible(builder, "(1,2,3)");
  shouldNotBeEligible(builder, "[1,2,3]");
  shouldNotBeEligible(builder, "set([1,2,3])");
  shouldNotBeEligible(builder, "{0: 1, 1: 2, 2: 3}");
}

/**
 * Launch all the tests
 */

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  Py_Initialize();
  int ret { RUN_ALL_TESTS() };
  Py_Finalize();
  return ret;
}
