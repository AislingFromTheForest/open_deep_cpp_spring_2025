#include "cjson.h"

static PyObject* loads(PyObject* self, PyObject* args)
{
    const char* json_c;

    if (!PyArg_ParseTuple(args, "s", &json_c))
    {
    	printf("ERROR: Failed to parse args\n");
        return NULL;
    }

    PyObject* dict = NULL;
    if (!(dict = PyDict_New())) 
	{
    	printf("ERROR: Failed to create Dict Object\n");
    	return NULL;
    }

	int err = 0;
    parse_json(json_c, dict, err);

	if (err == 1)
	{
		printf("ERROR: Failed to build key or value from str2int\n");
		return NULL;
	}
	else if (err == 2)
	{
		printf("ERROR: Failed to build key or value from str2str\n");
		return NULL;
	}

	return dict;
}

static PyObject* dumps(PyObject* self, PyObject* args)
{
	PyObject* dict;
	if (!PyArg_ParseTuple(args, "O", &dict))
	{
		PyErr_Format(PyExc_TypeError, "Argument is not valid");
		return nullptr;
	}

	PyObject* keys   = PyDict_Keys(dict);
	PyObject* values = PyDict_Values(dict);
	size_t size = PyDict_Size(dict);

	PyObject *key, *value;

	const char* c_str;
	long c_int;

	std::stringstream ss;
	ss << "{";

	for (size_t i = 0; i < size; ++i)
	{
		key = PyList_GetItem(keys, i);
    	c_str = PyUnicode_AsUTF8(key);

		ss << "\"" << c_str << "\": ";

		value = PyList_GetItem(values, i);
		if (PyLong_Check(value))
		{
			c_int = PyLong_AsLong(value);
			ss << c_int;
		}
		else
		{
			c_str = PyUnicode_AsUTF8(value);
			ss << "\"" << c_str << "\"";
		}
		
		if (i < size - 1)
		{
			ss << ", ";
		}
	}

	ss << "}" << '\0';
	
	PyObject* py_str = Py_BuildValue("s", ss.view().data());
	return py_str;
}

static PyMethodDef funcMethods[] =
{
    {"loads", loads, METH_VARARGS, "descr"},
	{"dumps", dumps, METH_VARARGS, "descr"},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef moduleDef = 
{
    PyModuleDef_HEAD_INIT,
    "cjson",
    "descr",
    -1,
    funcMethods
};

PyMODINIT_FUNC PyInit_cjson(void)
{
	Py_Initialize();
    return PyModule_Create(&moduleDef);
}
