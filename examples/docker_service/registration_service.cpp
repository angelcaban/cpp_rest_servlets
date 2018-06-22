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

#include <csignal>
#include <memory>
#include <utility>
#include <thread>

#include "registration_service.hpp"
#include "login_servlet.hpp"
#include "logout_servlet.hpp"
#include "register_servlet.hpp"

namespace
{
  volatile std::sig_atomic_t gSignalStatus = -1;
}

void signal_handler(int signal) {
  ::gSignalStatus = signal;
}

RegistrationService::RegistrationService() {
  controller_.registerServlet<LoginServlet>("/login");
  controller_.registerServlet<LogoutServlet>("/logout");
  controller_.registerServlet<LogoutServlet>("/register");
}

RegistrationService::~RegistrationService() {
  controller_.shutdown().wait();
}

void RegistrationService::runService(std::string const& listening_endpoint) {
  controller_.setEndpoint(listening_endpoint);
  controller_.accept().wait();
}

int main(int argc, char** argv) {
  std::signal(SIGABRT, signal_handler);
  std::signal(SIGFPE,  signal_handler);
  std::signal(SIGILL,  signal_handler);
  std::signal(SIGINT,  signal_handler);
  std::signal(SIGSEGV, signal_handler);
  std::signal(SIGTERM, signal_handler);

  static std::unique_ptr<RegistrationService> service{new RegistrationService};
  service->runService("http://127.0.0.1:8080");

  while (::gSignalStatus < 0) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}