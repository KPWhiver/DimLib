// copyptr.hpp
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

#ifndef COPYPTR_HPP
#define COPYPTR_HPP

#include <cstring>

namespace dim
{
  template <typename Type>
  class CopyPtr
  {
    protected:
      Type *d_ptr;

    public:
      CopyPtr();
      explicit CopyPtr(Type *ptr);
      CopyPtr(CopyPtr const &other);
      CopyPtr(CopyPtr &&tmp);

      CopyPtr &operator=(CopyPtr const &other);
      CopyPtr &operator=(CopyPtr &&tmp);

      Type* get();
      Type const *get() const;
      Type &operator*();
      Type *operator->();
      Type const &operator*() const;
      Type const *operator->() const;

      void reset(Type *ptr);

      virtual ~CopyPtr();
  };

  template <typename Type>
  class ClonePtr : public CopyPtr<Type>
  {
    using CopyPtr<Type>::d_ptr;

    public:
      ClonePtr() = default;
      explicit ClonePtr(Type *ptr);
      ClonePtr(ClonePtr const &other);
      ClonePtr(ClonePtr &&tmp);

      ClonePtr &operator=(ClonePtr const &other);
      ClonePtr &operator=(ClonePtr &&tmp);
  };

  template <typename Type>
  ClonePtr<Type>::ClonePtr(Type *ptr)
  :
    CopyPtr<Type>::CopyPtr(ptr)
  {
  }

  template <typename Type>
  ClonePtr<Type>::ClonePtr(ClonePtr const &other)
  :
    CopyPtr<Type>::CopyPtr(0)
  {
    if(other.d_ptr != 0)
      CopyPtr<Type>::reset(other.d_ptr->clone());
  }

  template <typename Type>
  ClonePtr<Type>::ClonePtr(ClonePtr &&tmp)
  :
    CopyPtr<Type>::CopyPtr(tmp.d_ptr)
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
  CopyPtr<Type>::CopyPtr()
  :
    d_ptr(0)
  {
  }

  template <typename Type>
  CopyPtr<Type>::CopyPtr(Type *ptr)
  :
    d_ptr(ptr)
  {
  }

  template <typename Type>
  CopyPtr<Type>::CopyPtr(CopyPtr const &other)
  :
    d_ptr(0)
  {
    if(other.d_ptr != 0)
      d_ptr = new Type(*other.d_ptr);
  }

  template <typename Type>
  CopyPtr<Type>::CopyPtr(CopyPtr &&tmp)
  :
    d_ptr(tmp.d_ptr)
  {
    tmp.d_ptr = 0;
  }

  template <typename Type>
  CopyPtr<Type> &CopyPtr<Type>::operator=(CopyPtr const &other)
  {
    CopyPtr<Type> tmp(other);
    std::swap(tmp.d_ptr, d_ptr);
    return *this;
  }

  template <typename Type>
  CopyPtr<Type> &CopyPtr<Type>::operator=(CopyPtr &&tmp)
  {
    std::swap(tmp.d_ptr, d_ptr);
    return *this;
  }

  template <typename Type>
  Type const *CopyPtr<Type>::get() const
  {
    return d_ptr;
  }

  template <typename Type>
  Type *CopyPtr<Type>::get()
  {
    return d_ptr;
  }

  template <typename Type>
  Type &CopyPtr<Type>::operator*()
  {
    return *d_ptr;
  }

  template <typename Type>
  Type *CopyPtr<Type>::operator->()
  {
    return d_ptr;
  }

  template <typename Type>
  Type const &CopyPtr<Type>::operator*() const
  {
    return *d_ptr;
  }

  template <typename Type>
  Type const *CopyPtr<Type>::operator->() const
  {
    return d_ptr;
  }

  template <typename Type>
  void CopyPtr<Type>::reset(Type *ptr)
  {
    if(d_ptr != 0)
      delete d_ptr;

    d_ptr = ptr;
  }

  template <typename Type>
  CopyPtr<Type>::~CopyPtr()
  {
    delete d_ptr;
  }

} /* namespace dim */
#endif /* COPYPTR_HPP_ */
