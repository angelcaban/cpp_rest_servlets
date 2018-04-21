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
    if (auto path = msg.relative_uri().path(); !path.empty()) {
      // Use or create the servlets
      auto cache = ServletCache::getInstance();
      for (auto & servlet : *cache) {
	if (servlet.second->doHandle(msg)) { return; }
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
