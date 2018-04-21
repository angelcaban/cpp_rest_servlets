#pragma once

#include <memory>
#include <utility>
#include <stdexcept>

#include "servlet_factory.hpp"

namespace restful_servlets {

  /*! In-Memory cache for all servlets provided by this Controller.
   *  This is a sealed class that provides a singleton for use for
   *  the lifetime of the Controller’s process. */
  class ServletCache {
  public:
    using abstract_type = restful_servlets::AbstractServlet;
    using id_to_hash    = abstract_type::IdType;
    using creator_fn    = std::function<abstract_type*(id_to_hash&&)>;

    using servlet_container =
      boost::container::flat_map<id_to_hash, std::unique_ptr<abstract_type>>;

    using size_type              = servlet_container::size_type;
    using mapped_type            = servlet_container::mapped_type;
    using iterator               = servlet_container::iterator;
    using const_iterator         = servlet_container::const_iterator;
    using reverse_iterator       = servlet_container::reverse_iterator;
    using const_reverse_iterator = servlet_container::const_reverse_iterator;

  private:
    ServletFactory<abstract_type*, id_to_hash, creator_fn> _factory;
    servlet_container _servlets;

    ServletCache() = default;
    static std::unique_ptr<ServletCache> _singleton;

  public:
    ~ServletCache() = default;

    size_type typesSize() const {
      return _factory.size();
    }
    size_type cacheSize() const {
      return _servlets.size();
    }

    iterator               begin()   { return _servlets.begin(); }
    const_iterator         cbegin()  { return _servlets.cbegin(); }
    iterator               end()     { return _servlets.end(); }
    const_iterator         cend()    { return _servlets.cend(); }
    reverse_iterator       rbegin()  { return _servlets.rbegin(); }
    const_reverse_iterator crbegin() { return _servlets.crbegin(); }
    reverse_iterator       rend()    { return _servlets.rend(); }
    const_reverse_iterator crend()   { return _servlets.crend(); }

    /*! Grab a reference to the Servlet at a specific id */
    mapped_type const& operator[](id_to_hash const& id) const {
      return _servlets.at(id);
    }

    /*! Grab a reference to the Servlet at a specific id */
    mapped_type const& at(id_to_hash const& id) const {
      return _servlets.at(id);
    }

    /*! Find or create a servlet with a given ID */
    abstract_type* createOrFindServlet(id_to_hash const& id);

    /*! Remove a servlet from a given ID
     *  from the cache. */
    void removeServlet(id_to_hash const& id) {
      if (auto i = _servlets.find(id); i != _servlets.end()) {
	_servlets.erase(i);
      }
    }

    /*! Register a type that can be used
     *  by the Servlet Factory */
    void registerType(id_to_hash && id, creator_fn maker) {
      _factory.register_maker(id, maker);
    }

    /*! Clear all data from fixtures and servlet cache */
    void clearAll() {
      _servlets.clear();
      _factory.clear();
    }

    ServletCache(ServletCache const&) = delete;
    ServletCache(ServletCache&&) = delete;

    static ServletCache* const getInstance();
  };

} // restful_servlets
