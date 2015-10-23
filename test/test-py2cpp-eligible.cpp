#include <Python.h>
#include "gtest/gtest.h"

#include <limits>
#include <memory>
#include <sstream>

#include "src/py2cpp.hpp"
#include "test/helper.hpp"

using namespace dubzzz::Py2Cpp;

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
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(pyQuery.c_str(), Py_eval_input, get_py_dict(), NULL) };
  EXPECT_TRUE(builder.eligible(pyo.get())) << "Conversion should be possible for: '" << pyQuery << "'";
  EXPECT_NO_THROW(filterThrow(builder, pyo.get())) << "Conversion should be possible for: '" << pyQuery << "'";;
}
template <class BUILDER>
static void shouldNotBeEligible(BUILDER const& builder, std::string const& pyQuery)
{
  std::unique_ptr<PyObject, decref> pyo { PyRun_String(pyQuery.c_str(), Py_eval_input, get_py_dict(), NULL) };
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

