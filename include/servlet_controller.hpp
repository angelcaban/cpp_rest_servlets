#pragma once

#include <servlet_cache.hpp>

namespace restful_servlets {

template <class T>
T* servlet_create(std::string&& s) {
  return new T(std::move(s));
}

class ServletController : public BasicController, Controller {
 public:
  ServletController() : BasicController() {
  }
  virtual ~ServletController() override = default;

  template <typename T>
  void attachServlet(std::string && path) {
    auto cache = ServletCache::getInstance();
    cache->registerType(std::move(path),
			std::bind(&servlet_create<T>, std::placeholders::_1));
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
