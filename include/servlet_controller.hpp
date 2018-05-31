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
#include <json_servlet.hpp>

namespace restful_servlets {

  using optional_parent = std::optional<std::reference_wrapper<std::string>>;

  template <class T>
  T* servlet_create(std::string&& s) {
    return new T(std::move(s));
  }

  class ServletController : public cfx::BasicController, cfx::IController {
    void buildId(std::string &id,
                 std::string &&path,
                 optional_parent parent = std::nullopt) {
      id.assign(std::move(path));
      if (parent) {
        id += ">" + parent->get();
      }
    }

  public:
    ServletController() : BasicController() {
    }
    ~ServletController() override = default;

    template <typename T>
    std::string registerServlet(std::string &&path,
                                optional_parent parent = std::nullopt) {
      auto &cache_ = ServletCache::getInstance();
      std::string id;
      buildId(id, std::move(path), std::move(parent));

      std::string ret = id;
      cache_.registerType(std::move(id),
                          std::bind(&servlet_create<T>,
                                    std::placeholders::_1));
      if (nullptr == cache_.createOrFindServlet(ret)) {
        std::string msg;
        msg += ret;
        msg += " is an invalid ID.";
        throw std::invalid_argument{msg};
      }

      return ret;
    }

    void registerJsonServlet(std::string &&path,
                             std::string &&config_file,
                             optional_parent parent = std::nullopt) {
      auto id = registerServlet<JsonServlet>(std::move(path),
                                             std::move(parent));
      auto &cache_ = ServletCache::getInstance();
      auto *servlet = dynamic_cast<JsonServlet *>(cache_.at(id).get());
      servlet->setConfigPath(std::move(config_file));
      servlet->loadConfig();
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
    /*! FYI: It’s not obvious from the interface, but
     *       http_request & http_response value type
     *       is a cheap copy of a single pimpl
     */
    void doHandle(http_request);
  };

} // namespace restful_servlets
