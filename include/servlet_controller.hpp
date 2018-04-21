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
