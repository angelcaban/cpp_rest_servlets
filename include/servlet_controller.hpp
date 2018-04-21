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

#include <utility>
#include <stdexcept>
#include <functional>
#include <servlet_cache.hpp>

namespace restful_servlets {

  using optional_parent = std::optional<std::reference_wrapper<std::string>>;

  template <class T>
  T* servlet_create(std::string&& s) {
    return new T(std::move(s));
  }

  class ServletController : public cfx::BasicController, cfx::IController {
  public:
    ServletController() : BasicController() {
    }
    virtual ~ServletController() override = default;

    template <typename T>
    void registerServlet(std::string && path,
			 optional_parent parent = std::nullopt) {
      auto cache = ServletCache::getInstance();
      std::string id{std::move(path)};
      if (parent) {
	id += ">" + parent->get();
      }
      cache->registerType(std::move(id),
			  std::bind(&servlet_create<T>,
				    std::placeholders::_1));
      if (nullptr == cache->createOrFindServlet(id)) {
	std::string msg;
	msg += id;
	msg += " is an invalid ID.";
	throw std::invalid_argument(msg);
      }
    }

    void handleGet(http_request) override;
    void handlePut(http_request) override;
    void handlePost(http_request) override;
    void handlePatch(http_request) override;
    void handleDelete(http_request) override;
    void handleHead(http_request) override;
    void handleOptions(http_request) override;
    void handleTrace(http_request) override;
    void handleConnect(http_request) override;
    void handleMerge(http_request) override;
    void initRestOpHandlers() override;

  private:
    void doHandle(http_request);
  };

} // namespace restful_servlets
