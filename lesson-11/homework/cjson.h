#include <Python.h>
#include <sstream>
#include "stdio.h"

void parse_json(const char* json, PyObject* dict, int& err);
