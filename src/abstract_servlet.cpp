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

#include "abstract_servlet.hpp"

using std::string;
using web::uri_builder;
using web::http::http_request;

namespace restful_servlets {

  AbstractServlet::AbstractServlet(const char* path)
    : path_{path} {
  }
  AbstractServlet::AbstractServlet(string && path)
    : path_{std::move(path)} {
  }

  bool AbstractServlet::doHandle(http_request req) {
    bool handled{false};
    auto const& p = req.request_uri().path();
    
    if (auto idx = path_.find('>'); idx != string::npos) {
      handled = handle(req);
    }
    if (handled) { return handled; }

    // If exact path match, handle request directly.
    if (p == path_) {
      return handle(req);
    }

    if (children_.empty()) { return handled; }

    // Is any part of request path owned by us?
    size_t i = 0;
    string subpath;
    for (; i < p.size(); ++i) {
      if (path_[i] != p[i]) { break; }
    }
    if (i < path_.size()) {
      return handled; // no sub-match
    }

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

    if (children_.size() < 100) {
      for (auto & child : children_) {
	if (handled = child.second->doHandle(req); handled) {
	  return handled;
	}
      }
    } else {
      // Let children have a go at it
      auto lowerMatches = children_.lower_bound(subpath);
      lowerMatches++;
      while ((lowerMatches != children_.begin()) &&
	     (!(handled = lowerMatches->second->doHandle(req)))) {
	--lowerMatches;
      }
    }

    return handled;
  }

} // namespace restful_servlets
