// base_iterator__.hpp
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

#ifndef BASEITERATOR_HPP__
#define BASEITERATOR_HPP__

#include <iterator>
#include <DIM/drawable.hpp>

namespace dim
{
  template<typename... RefType>
  class DrawableWrapper__;

  template<typename RetType, typename Container, typename IdType>
  class base_iterator__ : public std::iterator<std::forward_iterator_tag, RetType>
  {
      template <typename... RefType>
      friend class DrawableWrapper__;

      friend class DrawMap;

      IdType d_iter;
      Container *d_container;

    public:
      base_iterator__ &operator++();
      base_iterator__ const operator++(int);

      //base_iterator__ &operator--();
      //base_iterator__ const operator--(int);

      bool operator==(base_iterator__ const &rhs) const;
      bool operator!=(base_iterator__ const &rhs) const;

      RetType &operator*();
      RetType *operator->();

      RetType &operator*() const;
      RetType *operator->() const;

    private:
      base_iterator__(IdType const &iter, Container *container);
      IdType &id();
      IdType const &id() const;
      Container *container();
  };

  template<typename RetType, typename Container, typename IdType>
  IdType &base_iterator__<RetType, Container, IdType>::id()
  {
    return d_iter;
  }

  template<typename RetType, typename Container, typename IdType>
  Container *base_iterator__<RetType, Container, IdType>::container()
  {
    return d_container;
  }

  template<typename RetType, typename Container, typename IdType>
  base_iterator__<RetType, Container, IdType>::base_iterator__(IdType const &iter
      , Container *container)
      : d_iter(iter), d_container(container)
  {
  }

  template<typename RetType, typename Container, typename IdType>
  base_iterator__<RetType, Container, IdType> &base_iterator__<RetType, Container, IdType>::operator++()
  {
    d_container->nextId(d_iter);
    return *this;
  }

  template<typename RetType, typename Container, typename IdType>
  base_iterator__<RetType, Container, IdType> const base_iterator__<RetType, Container, IdType>::operator++(int)
  {
    base_iterator__<RetType, Container, IdType> ret(*this);
    d_container->nextId(d_iter);
    return ret;
  }

  //template<typename RetType, typename Container, typename IdType>
  //base_iterator__<RetType, Container, IdType> &base_iterator__<RetType, Container, IdType>::operator--()
  //{
  //  d_container->previousId(d_iter);
  //  return *this;
  //}

  //template<typename RetType, typename Container, typename IdType>
  //base_iterator__<RetType, Container, IdType> const base_iterator__<RetType, Container, IdType>::operator--(int)
  //{
  //  base_iterator__<RetType, Container, IdType> ret(*this);
  //  d_container->previousId(d_iter);
  //  return ret;
  //}

  template<typename RetType, typename Container, typename IdType>
  bool base_iterator__<RetType, Container, IdType>::operator==(base_iterator__<RetType, Container, IdType> const &rhs) const
  {
    return d_iter.first == rhs.d_iter.first && d_iter.second == rhs.d_iter.second;
  }

  template<typename RetType, typename Container, typename IdType>
  bool base_iterator__<RetType, Container, IdType>::operator!=(base_iterator__<RetType, Container, IdType> const &rhs) const
  {
    return d_iter.first != rhs.d_iter.first || d_iter.second != rhs.d_iter.second;
  }

  template<typename RetType, typename Container, typename IdType>
  RetType &base_iterator__<RetType, Container, IdType>::operator*()
  {
    return d_container->getFromId(d_iter);
  }

  template<typename RetType, typename Container, typename IdType>
  RetType *base_iterator__<RetType, Container, IdType>::operator->()
  {
    return &d_container->getFromId(d_iter);
  }

  template<typename RetType, typename Container, typename IdType>
  RetType &base_iterator__<RetType, Container, IdType>::operator*() const
  {
    return d_container->getFromId(d_iter);
  }

  template<typename RetType, typename Container, typename IdType>
  RetType *base_iterator__<RetType, Container, IdType>::operator->() const
  {
    return &d_container->getFromId(d_iter);
  }
}

#endif
