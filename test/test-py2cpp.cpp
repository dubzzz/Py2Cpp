#include "gtest/gtest.h"

#include <climits>
#include <Python.h>
#include "py2cpp.hpp"

using namespace dubzzz::Py2Cpp;
PyObject *py_main;
PyObject *py_dict;

TEST(CppBuilder_bool, True)
{
  PyObject* pyo { PyRun_String("True", Py_eval_input, py_dict, NULL) };
  EXPECT_EQ(true, CppBuilder<bool>()(pyo));
  Py_DECREF(pyo);
}
TEST(CppBuilder_bool, False)
{
  PyObject* pyo { PyRun_String("False", Py_eval_input, py_dict, NULL) };
  EXPECT_EQ(true, !CppBuilder<bool>()(pyo));
  Py_DECREF(pyo);
}

TEST(CppBuilder_int, AnyValue)
{
  PyObject* pyo { PyRun_String("5", Py_eval_input, py_dict, NULL) };
  EXPECT_EQ(5, CppBuilder<int>()(pyo));
  Py_DECREF(pyo);
}

TEST(CppBuilder_string, String)
{
  PyObject* pyo { PyRun_String("'hello'", Py_eval_input, py_dict, NULL) };
  EXPECT_EQ("hello", CppBuilder<std::string>()(pyo));
  Py_DECREF(pyo);
}

TEST(CppBuilder_string, Unicode)
{
  PyObject* pyo { PyRun_String("u'hello'", Py_eval_input, py_dict, NULL) };
  EXPECT_EQ("hello", CppBuilder<std::string>()(pyo));
  Py_DECREF(pyo);
}

TEST(CppBuilder_wstring, String)
{
  PyObject* pyo { PyRun_String("'hello'", Py_eval_input, py_dict, NULL) };
  EXPECT_EQ(L"hello", CppBuilder<std::wstring>()(pyo));
  Py_DECREF(pyo);
}

TEST(CppBuilder_wstring, Unicode)
{
  PyObject* pyo { PyRun_String("u'hello'", Py_eval_input, py_dict, NULL) };
  EXPECT_EQ(L"hello", CppBuilder<std::wstring>()(pyo));
  Py_DECREF(pyo);
}

TEST(CppBuilder_mix, AnyValue)
{
  PyObject* pyo { PyRun_String("{ \
        'positions':[ \
        { \
          'x': 5, \
          'y': 10, \
        }, { \
          'x': -1, \
          'y': 2, \
        }], \
      }", Py_eval_input, py_dict, NULL) };
  
  std::map<std::string, std::vector<std::map<std::string, int>>> elt;
  std::vector<std::map<std::string, int>> positions;
  positions.push_back({ {"x", 5}, {"y", 10} });
  positions.push_back({ {"x", -1}, {"y", 2} });
  elt["positions"] = positions;
  
  auto Functor = CppBuilder<std::map<std::string, std::vector<std::map<std::string, int>>>>();
  EXPECT_EQ(elt, Functor(pyo));
  Py_DECREF(pyo);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  Py_Initialize();
  py_main = PyImport_AddModule("__main__");
  py_dict = PyModule_GetDict(py_main);
  int ret { RUN_ALL_TESTS() };
  Py_Finalize();
  return ret;
}

