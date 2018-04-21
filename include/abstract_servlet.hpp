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
