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

TEST(make_cpp_bool, True)
{
  unique_ptr_ctn pyo { PyRun_String("True", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(true, make_cpp<bool>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}
TEST(make_cpp_bool, False)
{
  unique_ptr_ctn pyo { PyRun_String("False", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(true, !make_cpp<bool>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** int **/

TEST(make_cpp_int, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, make_cpp<int>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_int, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, make_cpp<int>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_int, MinValue)
{
  std::ostringstream out; out << INT_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(INT_MIN, make_cpp<int>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_int, MaxValue)
{
  std::ostringstream out; out << INT_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(INT_MAX, make_cpp<int>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_int, LessThanMinValue)
{
  std::ostringstream out; out << INT_MIN << "-1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<int>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_int, MoreThanMaxValue)
{
  std::ostringstream out; out << INT_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<int>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned int **/

TEST(make_cpp_uint, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, make_cpp<unsigned int>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_uint, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, make_cpp<unsigned int>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_uint, MaxValue)
{
  std::ostringstream out; out << UINT_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(UINT_MAX, make_cpp<unsigned int>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_uint, LessThanZero)
{
  unique_ptr_ctn pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned int>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_uint, MoreThanMaxValue)
{
  std::ostringstream out; out << UINT_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned int>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** long **/

TEST(make_cpp_long, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, make_cpp<long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_long, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, make_cpp<long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_long, MinValue)
{
  std::ostringstream out; out << LONG_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LONG_MIN, make_cpp<long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_long, MaxValue)
{
  std::ostringstream out; out << LONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LONG_MAX, make_cpp<long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_long, LessThanMinValue)
{
  std::ostringstream out; out << LONG_MIN << "-1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_long, MoreThanMaxValue)
{
  std::ostringstream out; out << LONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned long **/

TEST(make_cpp_ulong, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, make_cpp<unsigned long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ulong, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, make_cpp<unsigned long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ulong, MaxValue)
{
  std::ostringstream out; out << ULONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(ULONG_MAX, make_cpp<unsigned long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ulong, LessThanZero)
{
  unique_ptr_ctn pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ulong, FarLessThanZero)
{
  std::ostringstream out; out << LONG_MIN << "-1"; // in order to get a PyLong
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ulong, MoreThanMaxValue)
{
  std::ostringstream out; out << ULONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** long long **/

TEST(make_cpp_llong, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, make_cpp<long long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_llong, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, make_cpp<long long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_llong, MinValue)
{
  std::ostringstream out; out << LLONG_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LLONG_MIN, make_cpp<long long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_llong, MaxValue)
{
  std::ostringstream out; out << LLONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(LLONG_MAX, make_cpp<long long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_llong, LessThanMinValue)
{
  std::ostringstream out; out << LLONG_MIN << "-1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<long long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_llong, MoreThanMaxValue)
{
  std::ostringstream out; out << LLONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<long long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** unsigned long **/

TEST(make_cpp_ullong, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(0, make_cpp<unsigned long long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ullong, Any)
{
  unique_ptr_ctn pyo { PyRun_String("5", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(5, make_cpp<unsigned long long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ullong, MaxValue)
{
  std::ostringstream out; out << ULLONG_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(ULLONG_MAX, make_cpp<unsigned long long>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ullong, LessThanZero)
{
  unique_ptr_ctn pyo { PyRun_String("-1", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned long long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ullong, FarLessThanZero)
{
  std::ostringstream out; out << LONG_MIN << "-1"; // in order to get a PyLong
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned long long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_ullong, MoreThanMaxValue)
{
  std::ostringstream out; out << ULLONG_MAX << "+1";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<unsigned long long>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** double **/

TEST(make_cpp_double, Zero)
{
  unique_ptr_ctn pyo { PyRun_String("0", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(0, make_cpp<double>(pyo.get()), DBL_MIN); // PyFloat_GetMin() == DBL_MIN
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_double, Any)
{
  unique_ptr_ctn pyo { PyRun_String("3.14", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(3.14, make_cpp<double>(pyo.get()), 2*DBL_EPSILON);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_double, MinValue)
{
  std::ostringstream out; out << DBL_MIN;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_NEAR(PyFloat_GetMin(), make_cpp<double>(pyo.get()), DBL_MIN);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_double, MaxValue)
{
  std::ostringstream out; out << DBL_MAX;
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  const double tolerancy { pow(2, DBL_MAX_EXP) };
  EXPECT_NEAR(PyFloat_GetMax(), make_cpp<double>(pyo.get()), tolerancy);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_double, MoreThanMaxValue)
{
  std::ostringstream out; out << DBL_MAX << "*2";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<double>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_double, MoreThanMaxValueNegative)
{
  std::ostringstream out; out << DBL_MAX << "*(-2)";
  unique_ptr_ctn pyo { PyRun_String(out.str().c_str(), Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_THROW(make_cpp<double>(pyo.get()), std::overflow_error);
  EXPECT_FALSE(uncaught_exception());
}

/** std::string **/

TEST(make_cpp_string, String)
{
  unique_ptr_ctn pyo { PyRun_String("'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("hello", make_cpp<std::string>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_string, Unicode)
{
  unique_ptr_ctn pyo { PyRun_String("u'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("hello", make_cpp<std::string>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_string, UnicodeExotic)
{
  unique_ptr_ctn pyo { PyRun_String(
      "u'\u15c7\u25d8\u0034\u2b15'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ("\xe1\x97\x87\xe2\x97\x98\x34\xe2\xac\x95", make_cpp<std::string>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** std::wstring **/

TEST(make_cpp_wstring, String)
{
  unique_ptr_ctn pyo { PyRun_String("'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"hello", make_cpp<std::wstring>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_wstring, Unicode)
{
  unique_ptr_ctn pyo { PyRun_String("u'hello'", Py_eval_input, get_py_dict(), NULL) };
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"hello", make_cpp<std::wstring>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

TEST(make_cpp_wstring, UnicodeExotic)
{
  unique_ptr_ctn pyo { PyRun_String(
      "u'\u15c7\u25d8\u0034\u2b15'", Py_eval_input, get_py_dict(), NULL) }; //utf-16
  ASSERT_NE(nullptr, pyo.get());
  EXPECT_EQ(L"\xe1\x97\x87\xe2\x97\x98\x34\xe2\xac\x95", make_cpp<std::wstring>(pyo.get()));
  EXPECT_FALSE(uncaught_exception());
}

/** eligible_cpp **/

static std::string maxLL()
{
  std::ostringstream out;
  out << LLONG_MAX;
  return out.str();
}

template <class TYPE> static void filterThrow(PyObject* pyo)
{
  try
  {
    make_cpp<TYPE>(pyo);
  }
  catch(std::invalid_argument const&)
  {
    throw;
  }
  catch(...) {}
}

template <class TYPE> static void shouldBeEligible(std::string const& pyQuery)
{
  unique_ptr_ctn pyo { PyRun_String(pyQuery.c_str(), Py_eval_input, get_py_dict(), NULL) };
  EXPECT_TRUE(eligible_cpp<TYPE>(pyo.get())) << "Conversion should be possible for: '" << pyQuery << "'";
  EXPECT_NO_THROW(filterThrow<TYPE>(pyo.get())) << "Conversion should be possible for: '" << pyQuery << "'";;
}
template <class TYPE> static void shouldNotBeEligible(std::string const& pyQuery)
{
  unique_ptr_ctn pyo { PyRun_String(pyQuery.c_str(), Py_eval_input, get_py_dict(), NULL) };
  EXPECT_FALSE(eligible_cpp<TYPE>(pyo.get())) << "Conversion should not be possible for: '" << pyQuery << "'";
  EXPECT_THROW(make_cpp<TYPE>(pyo.get()), std::invalid_argument) << "Conversion should not be possible for: '" << pyQuery << "'";
}

TEST(eligible_cpp, bool)
{
  shouldBeEligible<bool>("True");
  
  shouldNotBeEligible<bool>("None");
  shouldNotBeEligible<bool>("1");
  shouldNotBeEligible<bool>(maxLL());
  shouldNotBeEligible<bool>("1.2");
  shouldNotBeEligible<bool>("'This is a string'");
  shouldNotBeEligible<bool>("u'This is a unicode string'");
  shouldNotBeEligible<bool>("(1,2,3)");
  shouldNotBeEligible<bool>("[1,2,3]");
  shouldNotBeEligible<bool>("set([1,2,3])");
  shouldNotBeEligible<bool>("{'x': 1, 'y': 2, 'z': 3}");
}

template <class TYPE>
static void testInteger()
{
  shouldBeEligible<TYPE>("True");
  shouldBeEligible<TYPE>("1");
  shouldBeEligible<TYPE>(maxLL());
  
  shouldNotBeEligible<TYPE>("None");
  shouldNotBeEligible<TYPE>("1.2");
  shouldNotBeEligible<TYPE>("'This is a string'");
  shouldNotBeEligible<TYPE>("u'This is a unicode string'");
  shouldNotBeEligible<TYPE>("(1,2,3)");
  shouldNotBeEligible<TYPE>("[1,2,3]");
  shouldNotBeEligible<TYPE>("set([1,2,3])");
  shouldNotBeEligible<TYPE>("{'x': 1, 'y': 2, 'z': 3}");
}

TEST(eligible_cpp, int)
{
  testInteger<int>();
}
TEST(eligible_cpp, unsigned_int)
{
  testInteger<unsigned int>();
}

TEST(eligible_cpp, long)
{
  testInteger<long>();
}
TEST(eligible_cpp, unsigned_long)
{
  testInteger<unsigned long>();
}

TEST(eligible_cpp, long_long)
{
  testInteger<long long>();
}
TEST(eligible_cpp, unsigned_long_long)
{
  testInteger<unsigned long long>();
}

TEST(eligible_cpp, double)
{
  shouldBeEligible<double>("True");
  shouldBeEligible<double>("1");
  shouldBeEligible<double>(maxLL());
  shouldBeEligible<double>("1.2");
  
  shouldNotBeEligible<double>("None");
  shouldNotBeEligible<double>("'This is a string'");
  shouldNotBeEligible<double>("u'This is a unicode string'");
  shouldNotBeEligible<double>("(1,2,3)");
  shouldNotBeEligible<double>("[1,2,3]");
  shouldNotBeEligible<double>("set([1,2,3])");
  shouldNotBeEligible<double>("{'x': 1, 'y': 2, 'z': 3}");
}

template <class TYPE>
static void testString()
{
  shouldBeEligible<TYPE>("'This is a string'");
  shouldBeEligible<TYPE>("u'This is a unicode string'");
  
  shouldNotBeEligible<TYPE>("None");
  shouldNotBeEligible<TYPE>("True");
  shouldNotBeEligible<TYPE>("1");
  shouldNotBeEligible<TYPE>(maxLL());
  shouldNotBeEligible<TYPE>("1.2");
  shouldNotBeEligible<TYPE>("(1,2,3)");
  shouldNotBeEligible<TYPE>("[1,2,3]");
  shouldNotBeEligible<TYPE>("set([1,2,3])");
  shouldNotBeEligible<TYPE>("{'x': 1, 'y': 2, 'z': 3}");
}

TEST(eligible_cpp, string)
{
  testString<std::string>();
}
TEST(eligible_cpp, wstring)
{
  testString<std::wstring>();
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
