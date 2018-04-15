#include "abstract_servlet.hpp"

using std::string;
using std::to_string;
using web::uri_builder;
using web::http::http_request;

namespace restful_servlets {

  AbstractServlet::AbstractServlet() {
  }
  AbstractServlet::AbstractServlet(const char* path)
    : path_(path) {
  }
  AbstractServlet::AbstractServlet(std::string const& path)
    : path_(path) {
  }
  AbstractServlet::AbstractServlet(std::string && path)
    : path_(std::move(path)) {
  }

  bool AbstractServlet::doHandle(http_request & req) {
    auto const& p = req.request_uri().path();
    if (p.size() > path_.size()) return false;

    // If exact path match, handle request directly.
    if (p == path_) return handle(req);

    // Is any part of request path owned by us?
    size_t i = 0;
    string subpath;
    for (; i < path_.size(); ++i) {
      if (path_[i] != p[i]) break;
    }
    if (i != path_.size()) return false; // no sub-match

    subpath = p.substr(i); // sub-path
    // Rebuild URI with the sub path
    uri_builder builder;
    builder.set_scheme(req.request_uri().scheme())
      .set_user_info(req.request_uri().user_info())
      .set_host(req.request_uri().host())
      .set_port(req.request_uri().port())
      .set_path(subpath)
      .set_query(req.request_uri().query())
      .set_fragment(req.request_uri().fragment());
    req.set_request_uri(builder.to_uri());

    // Let children have a go at it
    auto lowerMatches = children_.lower_bound(subpath);
    lowerMatches++;
    bool handled{false};
    while ((lowerMatches != children_.begin()) &&
	   (!(handled = lowerMatches->second->doHandle(req)))) {
      --lowerMatches;
    }

    return handled;
  }

} // namespace my_service
