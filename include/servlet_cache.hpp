#pragma once

#include <memory>
#include <utility>

#include "servlet_factory.hpp"

namespace restful_servlets {

  /*! In-Memory cache for all servlets provided by this Controller.
    This is a sealed class that provides a singleton for use for
    the lifetime of the Controller’s process. */
  class ServletCache {
  public:
    using abstract_type = restful_servlets::AbstractServlet;
    using id_to_hash = abstract_type::IdType;
    using creator_fn = std::function<abstract_type*(id_to_hash&&)>;
    using servlet_container =
      boost::container::flat_map<id_to_hash, std::unique_ptr<abstract_type>>;

  private:
    ServletFactory<abstract_type*, id_to_hash, creator_fn> _factory;

    servlet_container _servlets;

    ServletCache() = default;
    static std::unique_ptr<ServletCache> _singleton;

  public:
    ~ServletCache() = default;

    /*! Find or create a servlet with a given ID */
    template <class... args>
    abstract_type* createServlet(std::string&& id) {
      using std::make_pair;
      using std::unique_ptr;
      using std::forward;

      if (auto i = _servlets.find(id); i != _servlets.end())
	return i->second.get();

      auto* servlet = _factory.create(id);
      if (nullptr != servlet) {
	_servlets.insert(servlet_container::value_type{
	    std::move(id), unique_ptr<abstract_type>(servlet)});
      }
      return servlet;
    }

    /*! Remove a servlet from a given ID
     *  from the cache and return it. */
    void removeServlet(std::string const& id) {
      if (auto i = _servlets.find(id); i != _servlets.end()) {
	_servlets.erase(i);
      }
    }

    /*! Register a type that can be used
     *  by the Servlet Factory */
    void registerType(std::string&& id, creator_fn maker) {
      _factory.register_maker(id, maker);
    }

    ServletCache(ServletCache const&) = delete;
    ServletCache(ServletCache&&) = delete;

    static ServletCache* const getInstance();
  };

} // restful_servlets
