// cloneptr.hpp
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

#ifndef CLONEPTR_HPP
#define CLONEPTR_HPP

namespace dim
{
  template <typename Type>
  class ClonePtr
  {
    Type *d_ptr;

    public:
      ClonePtr();
      explicit ClonePtr(Type *ptr);
      ClonePtr(ClonePtr const &other);
      ClonePtr(ClonePtr &&tmp);

      ClonePtr &operator=(ClonePtr const &other);
      ClonePtr &operator=(ClonePtr &&tmp);

      operator Type*();
      Type &operator*() const;
      Type *operator->() const;

      void reset(Type *ptr);

      ~ClonePtr();
  };

  template <typename Type>
  ClonePtr<Type>::ClonePtr()
  :
    d_ptr(0)
  {
  }

  template <typename Type>
  ClonePtr<Type>::ClonePtr(Type *ptr)
  :
    d_ptr(ptr)
  {
  }

  template <typename Type>
  ClonePtr<Type>::ClonePtr(ClonePtr const &other)
  :
    d_ptr(0)
  {
    if(other.d_ptr != 0)
      d_ptr = other.d_ptr->clone();//new Type(*other.d_ptr);
  }

  template <typename Type>
  ClonePtr<Type>::ClonePtr(ClonePtr &&tmp)
  :
    d_ptr(tmp.d_ptr)
  {
    tmp.d_ptr = 0;
  }

  template <typename Type>
  ClonePtr<Type> &ClonePtr<Type>::operator=(ClonePtr const &other)
  {
    ClonePtr<Type> tmp(other);
    std::swap(tmp.d_ptr, d_ptr);
    return *this;
  }

  template <typename Type>
  ClonePtr<Type> &ClonePtr<Type>::operator=(ClonePtr &&tmp)
  {
    std::swap(tmp.d_ptr, d_ptr);
    return *this;
  }

  template <typename Type>
  ClonePtr<Type>::operator Type*()
  {
    return d_ptr;
  }

  template <typename Type>
  Type &ClonePtr<Type>::operator*() const
  {
    return *d_ptr;
  }

  template <typename Type>
  Type *ClonePtr<Type>::operator->() const
  {
    return d_ptr;
  }

  template <typename Type>
  void ClonePtr<Type>::reset(Type *ptr)
  {
    if(d_ptr != 0)
      delete d_ptr;

    d_ptr = ptr;
  }

  template <typename Type>
  ClonePtr<Type>::~ClonePtr()
  {
    delete d_ptr;
  }

} /* namespace dim */
#endif /* CLONEPTR_HPP_ */
