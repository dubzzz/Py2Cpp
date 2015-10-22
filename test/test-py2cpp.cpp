#include <Python.h>
#include "gtest/gtest.h"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  Py_Initialize();
  int ret { RUN_ALL_TESTS() };
  Py_Finalize();
  return ret;
}

