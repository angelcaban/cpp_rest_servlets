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

