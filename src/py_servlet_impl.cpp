/*
 * Copyright 2018 Angel Caban
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <json_servlet.hpp>
#include <py_defs.hpp>
#include <servlet_exceptions.hpp>

namespace restful_servlets {
  string extractException() {
    PyObject *exc,*val,*tb;
    PyErr_Fetch(&exc, &val, &tb);
    PyErr_NormalizeException(&exc, &val, &tb);
    handle<> hexc{exc};
    handle<> hval{allow_null(val)};
    handle<> htb{allow_null(tb)};
    if (!hval) {
      return extract<string>(str{hexc});
    }
    object traceback{import("traceback")};
    object format_exception{traceback.attr("format_exception")};
    object formatted_list{format_exception(hexc, hval, htb)};
    object formatted{str("").join(formatted_list)};
    return extract<string>(formatted);
  }

  bool forwardRequestToPython(http_request const& req,
			      string const& source) {
    try {
      Py_Initialize();
      object my_module{import("libpyservlet")};
      object main_module{import("__main__")};
      object main_namespace{main_module.attr("__dict__")};
      main_namespace["libpyservlet"] = my_module;
      main_namespace["status_codes"] = status_codes();
      main_namespace["methods"] = methods();
      main_namespace["request"] = req;

      (void) exec_file(source.c_str(),
		       main_namespace,
		       main_namespace);
      return extract<bool>(main_namespace["result"]);

    } catch (boost::python::error_already_set &) {
      string msg{extractException()};
      throw (ServletException(msg.c_str()));
    } catch (...) {
      throw (ServletException("Unknown error..."));
    }
  }
} // namespace restful_servlets
