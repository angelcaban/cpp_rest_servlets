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

#include <fstream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using std::string;
using std::ifstream;
using boost::filesystem::path;
using boost::filesystem::absolute;
using boost::property_tree::ptree;
using boost::property_tree::json_parser::read_json;
using web::http::http_request;

namespace restful_servlets {
  bool forwardRequestToPython(http_request const& req,
                              string const& source);

  JsonServlet::JsonServlet(const char* path)
    : AbstractServlet{path} {
  }
  JsonServlet::JsonServlet(string && path)
    : AbstractServlet{std::move(path)} {
  }

  void JsonServlet::loadConfig() {
    path p{config_file_};
    p = absolute(p);

    path parent_dir = p.parent_path();
    
    ifstream configStream{config_file_};
    if (!configStream) { return; }

    ptree configTree;
    read_json(configStream, configTree);

    servlet_name_ = configTree.get<string>("servlet.name");

    string impl{configTree.get<string>("servlet.type")};
    if (boost::iequals(impl, "python")) {
      impl_ = ServletImplmentation::PYTHON;
    } else if (boost::iequals(impl, "so")) {
      impl_ = ServletImplmentation::SO;
    }

    path source{configTree.get<string>("servlet.source")};
    if (source.is_relative()) {
      source = parent_dir;
      source /= configTree.get<string>("servlet.source");
    }
    impl_source_ = source.string();
  }

  bool JsonServlet::handle(http_request req) {
    switch (impl_) {
    case ServletImplmentation::PYTHON:
      return forwardRequestToPython(req, impl_source_);

    case ServletImplmentation::SO:
      // angelcaban: TODO
      return false;

    case ServletImplmentation::INVALID:
      return false;
    }

    return false;
  }

} // namespace restful_servlets
