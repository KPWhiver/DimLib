// wrapper_ptr.hpp
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

#ifndef WRAPPER_PTR_HPP
#define WRAPPER_PTR_HPP

namespace dim
{
  template <typename Type>
  class wrapper_ptr
  {
    Type *d_ptr;

    public:
      wrapper_ptr();
      wrapper_ptr(Type *ptr);
      wrapper_ptr(wrapper_ptr const &other);
      wrapper_ptr(wrapper_ptr &&tmp);

      wrapper_ptr &operator=(wrapper_ptr const &other);
      wrapper_ptr &operator=(wrapper_ptr &&tmp);

      operator Type*();
      Type &operator*();
      Type *operator->();

      ~wrapper_ptr();
  };

  template <typename Type>
  wrapper_ptr<Type>::wrapper_ptr()
  :
    d_ptr(0)
  {
  }

  template <typename Type>
  wrapper_ptr<Type>::wrapper_ptr(Type *ptr)
  :
    d_ptr(ptr)
  {
  }

  template <typename Type>
  wrapper_ptr<Type>::wrapper_ptr(wrapper_ptr const &other)
  :
    d_ptr(0)
  {
    if(other.d_ptr != 0)
      d_ptr = new Type(*other.d_ptr);
  }

  template <typename Type>
  wrapper_ptr<Type>::wrapper_ptr(wrapper_ptr &&tmp)
  :
    d_ptr(tmp.d_ptr)
  {
    tmp.d_ptr = 0;
  }

  template <typename Type>
  wrapper_ptr &wrapper_ptr<Type>::operator=(wrapper_ptr const &other)
  {
    wrapper_ptr<Type> tmp(other);
    std::swap(tmp, *this);
    return *this;
  }

  template <typename Type>
  wrapper_ptr &wrapper_ptr<Type>::operator=(wrapper_ptr &&tmp)
  {
    std::swap(tmp, *this);
    return *this;
  }

  template <typename Type>
  wrapper_ptr<Type>::operator Type*()
  {
    return d_ptr;
  }

  template <typename Type>
  Type &wrapper_ptr<Type>::operator*()
  {
    return *d_ptr;
  }

  template <typename Type>
  Type *wrapper_ptr<Type>::operator->()
  {
    return d_ptr;
  }

  template <typename Type>
  wrapper_ptr<Type>::~wrapper_ptr()
  {
    delete d_ptr;
  }

} /* namespace dim */
#endif /* PTRWRAPPER_HPP_ */
