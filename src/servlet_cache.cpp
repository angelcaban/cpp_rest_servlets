#include "servlet_cache.hpp"

namespace restful_servlets {

std::unique_ptr<ServletCache> ServletCache::_singleton;

ServletCache *const ServletCache::getInstance() {
  if (!_singleton) {
    _singleton.reset(new ServletCache);
  }
  return _singleton.get();
}

} // namespace restful_servlets
