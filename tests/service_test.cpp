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

  constexpr char endpoint[] = "http://localhost:33434";

  class ServiceFixture : public Test {
  public:
    ServletController controller;

    ServiceFixture() {
    }
    virtual ~ServiceFixture() override {
      ServletCache::getInstance().clearAll();
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
    virtual ~RootServlet() override = default;

    virtual bool handle(http_request req) override {
      utf8string body{BODY_STR};
      req.reply(status_codes::OK, std::move(body),
		"text/html; charset=utf-8");
      return true;
    }
  };

  class RootChildServlet : public AbstractServlet {
  public:
    constexpr static char BODY_STR[]{
      "{ \"child_of_root\" : true }"};

    RootChildServlet() : AbstractServlet() {}
    RootChildServlet(const char* path) : AbstractServlet(path) {}
    RootChildServlet(string const& path) : AbstractServlet(path) {}
    RootChildServlet(string&& path) : AbstractServlet(path) {}
    virtual ~RootChildServlet() override = default;

    virtual bool handle(http_request req) override {
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

    virtual bool handle(http_request req) override {
      utf8string body{BODY_STR};
      req.reply(status_codes::OK, std::move(body),
		"text/html; charset=utf-8");
      return true;
    }
  };

  TEST_F(ServiceFixture, DefaultCtor) {
    EXPECT_EQ(0u, ServletCache::getInstance().typesSize());
    EXPECT_EQ(0u, ServletCache::getInstance().cacheSize());
  }

  TEST_F(ServiceFixture, registerOneServlet) {
    controller.registerServlet<HelloWorldServlet>("/hello/world");

    EXPECT_EQ(1u, ServletCache::getInstance().typesSize());
    EXPECT_EQ(1u, ServletCache::getInstance().cacheSize());
  }

  TEST_F(ServiceFixture, registerTwoServlets) {
    controller.registerServlet<HelloWorldServlet>("/hello-world");
    controller.registerServlet<RootServlet>("/");

    EXPECT_EQ(2u, ServletCache::getInstance().typesSize());
    EXPECT_EQ(2u, ServletCache::getInstance().cacheSize());
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

    controller.shutdown().wait();
  }

  TEST_F(ServiceFixture, callBadUrl) {
    using web::http::client::http_client;
    using web::http::method;
    using web::http::methods;

    controller.registerServlet<HelloWorldServlet>("/hello-world");
    controller.setEndpoint(endpoint);
    controller.accept().wait();

    http_client client{{endpoint}};
    auto task1 = client.request(methods::GET, "/bad-url/");
    auto result1 = task1.wait();

    ASSERT_EQ(pplx::completed, result1);

    auto response = task1.get();
    auto task2 = response.extract_string();
    auto result2 = task2.wait();

    ASSERT_EQ(pplx::completed, result2);
    ASSERT_EQ("", task2.get());
    ASSERT_EQ(404, response.status_code());

    controller.shutdown().wait();
  }

  TEST_F(ServiceFixture, createChildServlet) {
    using web::http::client::http_client;
    using web::http::method;
    using web::http::methods;

    std::string rootUrl{"/hello"};
    std::string childUrl{"/world"};

    controller.registerServlet<RootServlet>(std::string{rootUrl});
    controller.registerServlet<RootChildServlet>(std::move(childUrl), rootUrl);

    controller.setEndpoint(endpoint);
    controller.accept().wait();

    http_client client{{endpoint}};
    auto task1 = client.request(methods::POST, "/hello");
    auto result1 = task1.wait();

    EXPECT_EQ(2u, ServletCache::getInstance().typesSize());
    EXPECT_EQ(2u, ServletCache::getInstance().cacheSize());
    EXPECT_EQ(1u, ServletCache::getInstance().at("/hello")->childrenCount());
    ASSERT_EQ(pplx::completed, result1);

    auto task2 = client.request(methods::POST, "/hello/world");
    auto result2 = task2.wait();
    ASSERT_EQ(pplx::completed, result2);

    auto response = task2.get();
    auto task3 = response.extract_string();
    auto result3 = task3.wait();
    ASSERT_EQ(pplx::completed, result3);

    ASSERT_EQ(RootChildServlet::BODY_STR, task3.get());

    controller.shutdown().wait();
  }

} // namespace

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
