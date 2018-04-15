#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <utility>

#include <cpprest/containerstream.h>
#include <basic_controller.hpp>
#include <runtime_utils.hpp>
#include <std_micro_service.hpp>
#include <usr_interrupt_handler.hpp>

#include <abstract_servlet.hpp>
#include <servlet_cache.hpp>

#include <gest/gtest.h>

using namespace web;
using namespace web::http;
using namespace cfx;
using namespace restful_servlets;
using namespace std;

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

namespace {

class RootServlet : public AbstractServlet {
 public:
  RootServlet() : AbstractServlet() {}
  RootServlet(const char* path) : AbstractServlet(path) {}
  RootServlet(string const& path) : AbstractServlet(path) {}
  RootServlet(string&& path) : AbstractServlet(path) {}

  virtual ~RootServlet() = default;

  virtual bool handle(http_request& req) override {
    utf8string body{"<html><body><h1>It Works!</h1></body></html>"};
    req.reply(status_codes::OK, std::move(body), "text/html; charset=utf-8");
    return true;
  }
};

class HelloWorldServlet : public AbstractServlet {
 public:
  HelloWorldServlet() : AbstractServlet() {}
  HelloWorldServlet(const char* path) : AbstractServlet(path) {}
  HelloWorldServlet(string const& path) : AbstractServlet(path) {}
  HelloWorldServlet(string&& path) : AbstractServlet(path) {}
  virtual ~HelloWorldServlet() override = default;

  virtual bool handle(http_request& req) override {
    utf8string body{"<html><body><strong>hello world</strong></body></html>"};
    req.reply(status_codes::OK, std::move(body), "text/html; charset=utf-8");
    return true;
  }
};

TEST(ServletController, DefaultCtor) {
  ServletController controller;

  EXPECT_EQ(ServletCache::getInstance()
}
  
} // namespace

int main(int argc, char** argv) {
  InterruptHandler::hookSIGINT();

  MyController server;
  server.setEndpoint("http://host_auto_ip4:33434");

  try {
    server.accept().wait();

    std::cout << "Now listening ’" << server.endpoint() << "’\n";

    InterruptHandler::waitForUserInterrupt();

    server.shutdown().wait();
  } catch (std::exception& e) {
    std::cerr << "Caught exception while trying to start the server - "
              << e.what() << "\n";
    return -1;
  } catch (...) {
    RuntimeUtils::printStackTrace();
  }

  return 0;
}
