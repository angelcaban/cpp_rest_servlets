#include "servlet_controller.hpp"

namespace restful_servlets {

void ServletController::initRestOpHandlers() {
  auto handler = [&](auto req) { doHandle(req); };
  _listener.support(methods::GET, handler);
  _listener.support(methods::POST, handler);
  _listener.support(methods::PUT, handler);
  _listener.support(methods::DEL, handler);
  _listener.support(methods::PATCH, handler);
}

void ServletController::doHandle(http_request msg) {
  if (auto path = uri::decode(msg.relative_uri().path()); !path.empty()) {
    // Use or create the servlets
    auto cache = ServletCache::getInstance();
    auto servlet = cache->createServlet(std::move(path));

    if (nullptr != servlet) {
      servlet->doHandle(msg);
      return;
    }
  }
  msg.reply(status_codes::NotFound);
}

void ServletController::handleGet(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handlePut(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handlePost(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handlePatch(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handleDelete(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handleHead(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handleOptions(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handleTrace(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handleConnect(http_request msg) {
  msg.reply(status_codes::NotFound);
}

void ServletController::handleMerge(http_request msg) {
  msg.reply(status_codes::NotFound);
}
  
} // namespace restful_servlets
