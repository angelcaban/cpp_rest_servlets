#pragma once

#include <type_traits>
#include <boost/container/flat_map.hpp>
#include "abstract_servlet.hpp"

namespace restful_servlets {

  template<class Servlet, class Sid, class MakeType>
  struct ServletFactory : public boost::container::flat_map<Sid, MakeType> {
    /*! Register a create function to the Sevlet Factory.
     *  We require that the create function turns a pointer
     *  of type ‘Servlet‘. Map the create function to a 
     *  unique identifier. */
    constexpr void register_maker(Sid id, MakeType make) {
      this->insert(std::make_pair(std::move(id), make));
    }

    /*! Create a new instance of a ‘Servlet‘ given a unique
     *  identifier. If the creator was never registered, a
     *  null pointer will be returned. */
    typename std::remove_pointer<Servlet>::type*
    create(Sid id) const {
      if (auto i = this->find(id); i != this->end())
	return i->second(std::move(id));
      return nullptr;
    }
  };

} // namespace restful_servlets

