// ptrvector.hpp
//
// Copyright 2012 Klaas Winter <klaaswinter@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA 02110-1301, USA.

#ifndef TUPLEFOREACH_HPP
#define TUPLEFOREACH_HPP

#include <tuple>

namespace dim
{
namespace internal
{

template<size_t Index, typename FunctionObject, typename... Types>
struct ForEach
{
  ForEach<Index - 1, FunctionObject, Types...> d_next;

  ForEach(std::tuple<Types...> &list, FunctionObject &object)
    :
      d_next(list, object)
  {
    object(std::get<Index>(list));
  }
};

template<typename FunctionObject, typename... Types>
struct ForEach<0, FunctionObject, Types...>
{
  ForEach(std::tuple<Types...> &list, FunctionObject &object)
  {
    object(std::get<0>(list));
  }
};
}

template<typename FunctionObject, typename... Types>
void forEach(std::tuple<Types...> &list, FunctionObject &object)
{
  internal::ForEach<std::tuple_size<std::tuple<Types...>>::value - 1, FunctionObject, Types...>{list, object};
}

template<typename FunctionObject, typename... Types>
void forEach(std::tuple<Types...> &list, FunctionObject &&object)
{
  internal::ForEach<std::tuple_size<std::tuple<Types...>>::value - 1, FunctionObject, Types...>{list, object};
}

namespace internal
{
  template<size_t Index, typename Type, typename... Types>
  struct TypeIndex;


  template<size_t Index, typename Type, typename FirstType, typename... Types>
  struct TypeIndex<Index, Type, FirstType, Types...>
  {
    static size_t const value = (std::is_same<Type, FirstType>::value ? Index : TypeIndex<Index + 1, Type, Types...>::value);
  };

  template<size_t Index, typename Type>
  struct TypeIndex<Index, Type>
  {
    static size_t const value = Index;
    //static_assert(true, "requested type was not found in tuple");
  };
}

template<typename Type, typename... Types>
Type &get(std::tuple<Types...> &tuple)
{
  return std::get<internal::TypeIndex<0, Type, Types...>::value>(tuple);
}

}

#endif
