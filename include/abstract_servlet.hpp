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

#pragma once

#include <basic_controller.hpp>
#include <map>
#include <string>
#include <utility>

namespace restful_servlets {

/*! This is the base class for every servlet
 *  that is supported by the running server.
 *
 *  Servlet objects are invoked via the
 *  handle() method.
 */
class AbstractServlet {
 public:
  using ChildServletMap = std::map<std::string, AbstractServlet*>;
  using ChildServletType = ChildServletMap::value_type;
  using IdType = std::string;

 private:
  ChildServletMap children_;
  IdType path_;

 public:
  AbstractServlet();
  AbstractServlet(const char* path);
  AbstractServlet(std::string const& path);
  AbstractServlet(std::string&& path);
  virtual ~AbstractServlet() = default;

  IdType const& getPath() const { return path_; }
  void setPath(const char* path) { path_ = path; }
  void setPath(IdType const& path) { path_ = path; }
  void setPath(IdType && path) { path_ = std::move(path); }

  void addChild(std::string const& subpath, AbstractServlet* child) {
    children_.insert(std::make_pair(subpath, child));
  }

  ChildServletMap::size_type childrenCount() const {
    return children_.size();
  }

  bool doHandle(web::http::http_request& req);
  virtual bool handle(web::http::http_request& req) = 0;
};

}  // namespace restful_servlets
