#include "cjson.h"
#include <algorithm>

std::string parse_key(std::string& key);
std::string parse_value(std::string& value, bool &is_digit);

void parse_json(const char* json, PyObject* dict, int& err)
{
    std::string key_c, value_c;

    PyObject* key_py = NULL;
	PyObject* value_py = NULL;

    std::stringstream ss;
    ss << json; 

    // remove first bracket;
    if (ss.get() != '{')
    {
        return;
    }

    // if {} -> return;
    if (ss.peek() == '}')
    {
        return;
    }

    bool is_digit;
    while (ss >> key_c && ss >> value_c)
    {
        key_c   = parse_key(key_c);
        value_c = parse_value(value_c, is_digit);

        if (is_digit)
        {
            long long_c = std::stoi(value_c);

            if ((key_py   = Py_BuildValue("s", key_c.c_str())) &&
                (value_py = Py_BuildValue("l", long_c) ))
            {
                PyDict_SetItem(dict, key_py, value_py);
            }
            else
            {
                err = 1;
                return;
            }
        }
        else
        {
            if ((key_py   = Py_BuildValue("s",   key_c.c_str())) && 
                (value_py = Py_BuildValue("s", value_c.c_str())))
            {
                PyDict_SetItem(dict, key_py, value_py);
            }
            else
            {
                err = 2;
                return;
            }
        }
    }

    return;
}

std::string parse_key(std::string& key)
{
    size_t l = key.find('\"');
    size_t r = key.rfind('\"');
    return std::string(key, l + 1, r - l - 1);
}

std::string parse_value(std::string& value, bool &is_digit)
{
    size_t l = value.find('\"');
    size_t r = value.rfind('\"');

    if (l != value.npos && r != value.npos)
    {
        is_digit = false;
        return std::string(value, l + 1, r - l - 1);
    }

    is_digit = true;
    
    l = 0;
    r = std::isdigit(value[value.size() - 1]) ? value.size() : value.size() - 1;

    return std::string(value, l, r - l);
}
