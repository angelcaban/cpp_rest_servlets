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

#include "servlet_cache.hpp"

using ServletCachePtr = std::unique_ptr<restful_servlets::ServletCache>;

using std::unique_ptr;
using std::string;
using std::vector;
using web::uri;

namespace restful_servlets {
  ServletCachePtr ServletCache::_singleton;

  ServletCache *const ServletCache::getInstance() {
    if (!_singleton) {
      _singleton.reset(new ServletCache);
    }
    return _singleton.get();
  }

  ServletCache::abstract_type*
  ServletCache::createOrFindServlet(id_to_hash const& id) {
    if (auto i = _servlets.find(id); i != _servlets.end()) {
      return i->second.get();
    }

    id_to_hash id_{id};
    std::optional<string> parent_id_;

    string::size_type sep_index = id_.find('>');
    if (sep_index != string::npos) {
      parent_id_ = uri::encode_uri(id_.substr(sep_index + 1));
      id_ = uri::encode_uri(id_.substr(0, sep_index));
    }

    auto* servlet = _factory.create(id);

    if (parent_id_ && nullptr != servlet) {
      if (auto parent_iter = _servlets.find(*parent_id_);
	  parent_iter != _servlets.end()) {
	parent_iter->second->addChild(id_, servlet);
      } else {
	std::string what = "Parent ID \"";
	what += *parent_id_;
	what += "\" is invalid.";
	throw std::invalid_argument{what};
      }
    }
    if (nullptr != servlet) {
      _servlets.insert(servlet_container::value_type{
	  std::move(id), unique_ptr<abstract_type>(servlet)});
    }

    return servlet;
  }
} // namespace restful_servlets
