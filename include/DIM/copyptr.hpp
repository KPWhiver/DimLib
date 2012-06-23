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

namespace dim
{
  template <typename Type>
  class CopyPtr
  {
    Type *d_ptr;
    size_t d_size;

    public:
      CopyPtr();
      CopyPtr(Type *ptr, size_t size = 1);
      CopyPtr(CopyPtr const &other);
      CopyPtr(CopyPtr &&tmp);

      CopyPtr &operator=(CopyPtr const &other);
      CopyPtr &operator=(CopyPtr &&tmp);

      operator Type*();
      Type &operator*();
      Type *operator->();

      ~CopyPtr();
  };

  template <typename Type>
  CopyPtr<Type>::CopyPtr()
  :
    d_ptr(0),
    d_size(1)
  {
  }

  template <typename Type>
  CopyPtr<Type>::CopyPtr(Type *ptr, size_t size)
  :
    d_ptr(ptr),
    d_size(size)
  {
  }

  template <typename Type>
  CopyPtr<Type>::CopyPtr(CopyPtr const &other)
  :
    d_ptr(0),
    d_size(other.d_size)
  {
    if(other.d_ptr != 0)
    {
      if(d_size > 1)
      {
        d_ptr = new Type[d_size];
        std::memcpy(other.d_ptr, d_ptr, d_size);
      }
      else
        d_ptr = new Type(*other.d_ptr);
    }
  }

  template <typename Type>
  CopyPtr<Type>::CopyPtr(CopyPtr &&tmp)
  :
    d_ptr(tmp.d_ptr),
    d_size(tmp.d_size)
  {
    tmp.d_ptr = 0;
    tmp.d_size = 1;
  }

  template <typename Type>
  CopyPtr<Type> &CopyPtr<Type>::operator=(CopyPtr const &other)
  {
    CopyPtr<Type> tmp(other);
    std::swap(tmp.d_ptr, d_ptr);
    std::swap(tmp.d_size, d_size);
    return *this;
  }

  template <typename Type>
  CopyPtr<Type> &CopyPtr<Type>::operator=(CopyPtr &&tmp)
  {
    std::swap(tmp.d_ptr, d_ptr);
    std::swap(tmp.d_size, d_size);
    return *this;
  }

  template <typename Type>
  CopyPtr<Type>::operator Type*()
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
  CopyPtr<Type>::~CopyPtr()
  {
    if(d_size > 1)
      delete[] d_ptr;
    else
      delete d_ptr;
  }

} /* namespace dim */
#endif /* COPYPTR_HPP_ */
