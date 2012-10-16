// iteratorbase.hpp
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

#ifndef ITERATORBASE_HPP__
#define ITERATORBASE_HPP__

#include <iterator>
#include <dim/drawable.hpp>

namespace dim
{
namespace internal
{
  template<typename RetType, typename Container, typename IterType>
  class IteratorBase : public std::iterator<std::forward_iterator_tag, RetType>
  {
      friend Container;

      IterType d_iter;
      Container *d_container;

    public:
      IteratorBase &operator++();
      IteratorBase operator++(int);

      bool operator==(IteratorBase const &rhs) const;
      bool operator!=(IteratorBase const &rhs) const;

      RetType &operator*();
      RetType *operator->();

      RetType &operator*() const;
      RetType *operator->() const;

    private:
      IteratorBase(IterType const &iter, Container *container);
      IterType &iter();
      IterType const &iter() const;
      Container *container();
  };

  template<typename RetType, typename Container, typename IterType>
  IterType &IteratorBase<RetType, Container, IterType>::iter()
  {
    return d_iter;
  }

  template<typename RetType, typename Container, typename IterType>
  Container *IteratorBase<RetType, Container, IterType>::container()
  {
    return d_container;
  }

  template<typename RetType, typename Container, typename IterType>
  IteratorBase<RetType, Container, IterType>::IteratorBase(IterType const &iter
      , Container *container)
      : d_iter(iter), d_container(container)
  {
  }

  template<typename RetType, typename Container, typename IterType>
  IteratorBase<RetType, Container, IterType> &IteratorBase<RetType, Container, IterType>::operator++()
  {
    d_iter = d_container->increment(&d_iter);
    return *this;
  }

  template<typename RetType, typename Container, typename IterType>
  IteratorBase<RetType, Container, IterType> IteratorBase<RetType, Container, IterType>::operator++(int)
  {
    IteratorBase<RetType, Container, IterType> ret(d_container->increment(&d_iter), d_container);
    return ret;
  }

  template<typename RetType, typename Container, typename IterType>
  bool IteratorBase<RetType, Container, IterType>::operator==(IteratorBase<RetType, Container, IterType> const &rhs) const
  {
    return d_container->equal(d_iter, rhs.d_iter);
  }

  template<typename RetType, typename Container, typename IterType>
  bool IteratorBase<RetType, Container, IterType>::operator!=(IteratorBase<RetType, Container, IterType> const &rhs) const
  {
    return not d_container->equal(d_iter, rhs.d_iter);
  }

  template<typename RetType, typename Container, typename IterType>
  RetType &IteratorBase<RetType, Container, IterType>::operator*()
  {
    return d_container->dereference(d_iter);
  }

  template<typename RetType, typename Container, typename IterType>
  RetType *IteratorBase<RetType, Container, IterType>::operator->()
  {
    return &d_container->dereference(d_iter);
  }

  template<typename RetType, typename Container, typename IterType>
  RetType &IteratorBase<RetType, Container, IterType>::operator*() const
  {
    return d_container->dereference(d_iter);
  }

  template<typename RetType, typename Container, typename IterType>
  RetType *IteratorBase<RetType, Container, IterType>::operator->() const
  {
    return &d_container->dereference(d_iter);
  }
}
}

#endif
