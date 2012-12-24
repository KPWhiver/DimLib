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

namespace dim
{
namespace internal
{
  template<typename RetType, typename Container, typename Iterable>
  class IteratorBase : public std::iterator<std::forward_iterator_tag, RetType>
  {
      friend Container;

      template<typename RefType>
      friend class NodeGrid;

      Iterable d_iterable;

    public:
      IteratorBase &operator++();
      IteratorBase operator++(int);

      bool operator==(IteratorBase const &other) const;
      bool operator!=(IteratorBase const &other) const;

      RetType &operator*();
      RetType *operator->();

      RetType const &operator*() const;
      RetType const *operator->() const;

    private:
      explicit IteratorBase(Iterable const &iterable);
      Iterable &iterable();
      Iterable const &iterable() const;
  };

  template<typename RetType, typename Container, typename Iterable>
  Iterable &IteratorBase<RetType, Container,Iterable>::iterable()
  {
    return d_iterable;
  }

  template<typename RetType, typename Container, typename Iterable>
  IteratorBase<RetType, Container,Iterable>::IteratorBase(Iterable const &iterable)
      : d_iterable(iterable)
  {
  }

  template<typename RetType, typename Container, typename Iterable>
  IteratorBase<RetType, Container,Iterable> &IteratorBase<RetType, Container,Iterable>::operator++()
  {
    d_iterable->increment();
    return *this;
  }

  template<typename RetType, typename Container, typename Iterable>
  IteratorBase<RetType, Container,Iterable> IteratorBase<RetType, Container,Iterable>::operator++(int)
  {
    Iterable iterable(d_iterable);
    iterable->increment();
    IteratorBase<RetType, Container, Iterable> ret(iterable);
    return ret;
  }

  template<typename RetType, typename Container, typename Iterable>
  bool IteratorBase<RetType, Container,Iterable>::operator==(IteratorBase<RetType, Container,Iterable> const &other) const
  {
    return d_iterable->equal(other.d_iterable);
  }

  template<typename RetType, typename Container, typename Iterable>
  bool IteratorBase<RetType, Container,Iterable>::operator!=(IteratorBase<RetType, Container,Iterable> const &other) const
  {
    return not d_iterable->equal(other.d_iterable);
  }

  template<typename RetType, typename Container, typename Iterable>
  RetType &IteratorBase<RetType, Container,Iterable>::operator*()
  {
    return d_iterable->dereference();
  }

  template<typename RetType, typename Container, typename Iterable>
  RetType *IteratorBase<RetType, Container,Iterable>::operator->()
  {
    return &d_iterable->dereference();
  }

  template<typename RetType, typename Container, typename Iterable>
  RetType const &IteratorBase<RetType, Container,Iterable>::operator*() const
  {
    return d_iterable->dereference();
  }

  template<typename RetType, typename Container, typename Iterable>
  RetType const *IteratorBase<RetType, Container,Iterable>::operator->() const
  {
    return &d_iterable->dereference();
  }
}
}

#endif
