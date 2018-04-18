#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <utility>

#include <cpprest/containerstream.h>
#include <cpprest/http_msg.h>
#include <cpprest/http_client.h>

#include <rest_service.hpp>
#include <servlet_controller.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace web;
using namespace web::http;
using namespace cfx;

using restful_servlets::ServletController;
using restful_servlets::ServletCache;
using restful_servlets::AbstractServlet;

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;

namespace {

  class ServiceFixture : public Test {
  public:
    ServletController controller;

    ServiceFixture() {
    }
    virtual ~ServiceFixture() override {
      ServletCache::getInstance()->clearAll();
    }
  };

  class RootServlet : public AbstractServlet {
  public:
    constexpr static char BODY_STR[]{
      "<html><body><h1>It Works!</h1></body></html>"};

    RootServlet() : AbstractServlet() {}
    RootServlet(const char* path) : AbstractServlet(path) {}
    RootServlet(string const& path) : AbstractServlet(path) {}
    RootServlet(string&& path) : AbstractServlet(path) {}
    virtual ~RootServlet() = default;

    virtual bool handle(http_request& req) override {
      utf8string body{BODY_STR};
      req.reply(status_codes::OK, std::move(body),
		"text/html; charset=utf-8");
      return true;
    }
  };

  class HelloWorldServlet : public AbstractServlet {
  public:
    constexpr static char BODY_STR[]{
      "<html><body><strong>hello world</strong></body></html>"};

    HelloWorldServlet() : AbstractServlet() {}
    HelloWorldServlet(const char* path) : AbstractServlet(path) {}
    HelloWorldServlet(string const& path) : AbstractServlet(path) {}
    HelloWorldServlet(string&& path) : AbstractServlet(path) {}
    virtual ~HelloWorldServlet() override = default;

    virtual bool handle(http_request& req) override {
      utf8string body{BODY_STR};
      req.reply(status_codes::OK, std::move(body),
		"text/html; charset=utf-8");
      return true;
    }
  };

  TEST_F(ServiceFixture, DefaultCtor) {
    EXPECT_NE(nullptr, ServletCache::getInstance());
    EXPECT_EQ(0u, ServletCache::getInstance()->typesSize());
    EXPECT_EQ(0u, ServletCache::getInstance()->cacheSize());
  }

  TEST_F(ServiceFixture, registerOneServlet) {
    controller.registerServlet<HelloWorldServlet>("/hello/world");

    EXPECT_NE(nullptr, ServletCache::getInstance());
    EXPECT_EQ(1u, ServletCache::getInstance()->typesSize());
    EXPECT_EQ(0u, ServletCache::getInstance()->cacheSize());
  }

  TEST_F(ServiceFixture, registerTwoServlets) {
    controller.registerServlet<HelloWorldServlet>("/hello-world");
    controller.registerServlet<RootServlet>("/");

    EXPECT_NE(nullptr, ServletCache::getInstance());
    EXPECT_EQ(2u, ServletCache::getInstance()->typesSize());
    EXPECT_EQ(0u, ServletCache::getInstance()->cacheSize());
  }

  TEST_F(ServiceFixture, createServlets) {
    using web::http::client::http_client;
    using web::http::method;
    using web::http::methods;

    controller.registerServlet<HelloWorldServlet>("/hello-world");
    controller.registerServlet<RootServlet>("/");
    controller.setEndpoint("http://localhost:33434");
    controller.accept().wait();

    http_client client{{"http://localhost:33434"}};
    {
      auto task1 = client.request(methods::GET, "/");
      auto result1 = task1.wait();
      ASSERT_EQ(pplx::completed, result1);

      auto response = task1.get();
      auto task2 = response.extract_string();
      auto result2 = task2.wait();
      ASSERT_EQ(pplx::completed, result2);

      ASSERT_EQ(RootServlet::BODY_STR, task2.get());
    }
    {
      auto task1 = client.request(methods::GET, "/hello-world");
      auto result1 = task1.wait();
      ASSERT_EQ(pplx::completed, result1);

      auto response = task1.get();
      auto task2 = response.extract_string();
      auto result2 = task2.wait();
      ASSERT_EQ(pplx::completed, result2);

      ASSERT_EQ(HelloWorldServlet::BODY_STR, task2.get());
    }
  }

  TEST_F(ServiceFixture, callBadUrl) {
    using web::http::client::http_client;
    using web::http::method;
    using web::http::methods;

    controller.registerServlet<HelloWorldServlet>("/hello-world");
    controller.setEndpoint("http://localhost:33434");
    controller.accept().wait();

    http_client client{{"http://localhost:33434"}};
    auto task1 = client.request(methods::GET, "/bad-url/");
    auto result1 = task1.wait();

    ASSERT_EQ(pplx::completed, result1);

    auto response = task1.get();
    auto task2 = response.extract_string();
    auto result2 = task2.wait();

    ASSERT_EQ(pplx::completed, result2);
    ASSERT_EQ("", task2.get());
    ASSERT_EQ(404, response.status_code());
  }

} // namespace

int main(int argc, char** argv) {
  /*
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
  */
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
