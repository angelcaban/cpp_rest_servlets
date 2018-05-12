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

#include <string>

#include <cpprest/containerstream.h>
#include <cpprest/http_msg.h>
#include <cpprest/http_client.h>

#include <boost/python.hpp>

using std::string;
using boost::python::class_;
using boost::python::range;
using boost::python::return_value_policy;
using boost::python::copy_const_reference;
using boost::python::allow_null;
using boost::python::exec_file;
using boost::python::borrowed;
using boost::python::extract;
using boost::python::object;
using boost::python::import;
using boost::python::handle;
using boost::python::scope;
using boost::python::str;
using boost::python::ptr;

using web::uri;
using web::http::methods;
using web::http::status_code;
using web::http::status_codes;
using web::http::http_headers;
using web::http::http_request;

#ifdef _XPLATSTR
#undef _XPLATSTR
#endif

#define STRINGIFY(x) #x
#define _XPLATSTR(x) std::string(x)

using headers_overload = const http_headers&(http_request::*)() const;
using reply = pplx::task<void>(http_request::*)(status_code, const string&, const string&) const;
using copy_const_policy = return_value_policy<copy_const_reference>;

BOOST_PYTHON_MODULE(libpyservlet) {

  class_<status_codes>("status_codes")
#define _PHRASES
#define DAT(a,b,c)    .def_readonly(STRINGIFY(a), &status_codes::a)
#include "cpprest/details/http_constants.dat"
#undef _PHRASES
#undef DAT
    ;

  class_<methods>("methods")
#define _METHODS
#define DAT(a,b)    .def_readonly(STRINGIFY(a), &methods::a)
#include "cpprest/details/http_constants.dat"
#undef _METHODS
#undef DAT
    ;

  class_<http_headers>("http_headers")
    .def("has",      &http_headers::has)
    .def("size",     &http_headers::size)
    .def("empty",    &http_headers::empty)
    .def("__iter__", boost::python::iterator<http_headers>());

  class_<uri>("uri")
    .def("scheme",    &uri::scheme,    copy_const_policy())
    .def("user_info", &uri::user_info, copy_const_policy())
    .def("host",      &uri::host,      copy_const_policy())
    .def("path",      &uri::path,      copy_const_policy())
    .def("query",     &uri::query,     copy_const_policy())
    .def("fragment",  &uri::fragment,  copy_const_policy())
    .def("port",      &uri::port)
    .def("is_host_loopback", &uri::is_host_loopback)
    .def("is_host_wildcard", &uri::is_host_wildcard)
    .def("is_host_portable", &uri::is_host_portable)
    .def("is_port_default",  &uri::is_port_default)
    .def("is_authority",     &uri::is_authority)
    .def("is_path_empty",    &uri::is_path_empty);

  class_<http_request>("http_request")
    .def("method",        &http_request::method,
         copy_const_policy())
    .def("request_uri",   &http_request::request_uri,
         copy_const_policy())
    .def<headers_overload>("headers",
                           &http_request::headers,
                           copy_const_policy())
    .def("http_version",   &http_request::http_version)
    .def("remote_address", &http_request::remote_address,
         copy_const_policy())
    .def("reply", static_cast<reply>(&http_request::reply));

}
