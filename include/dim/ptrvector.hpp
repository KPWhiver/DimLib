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

#ifndef PTRVECTOR_HPP
#define PTRVECTOR_HPP

#include <vector>
#include <functional>

namespace dim
{
  template <typename Type>
  class PtrVector : public std::vector<Type*>
  {
    std::function<Type*(Type*)> d_copyFunction;

    static Type* defaultCopyFunction(Type* ptr);

    public:
      explicit PtrVector(std::function<Type*(Type*)> const &copyFunction = defaultCopyFunction);
      explicit PtrVector(size_t count, std::function<Type*(Type*)> const &copyFunction = defaultCopyFunction);

      template<typename InputIt>
      PtrVector(InputIt first, InputIt last, std::function<Type*(Type*)> const &copyFunction = defaultCopyFunction);

      PtrVector(PtrVector const &other);
      PtrVector(PtrVector &&other);

      PtrVector &operator=(PtrVector const &other);
      PtrVector &operator=(PtrVector &&tmp);

      ~PtrVector();
  };

  template<typename Type>
  Type* PtrVector<Type>::defaultCopyFunction(Type* ptr)
  {
    return new Type(*ptr);
  }

  template<typename Type>
  PtrVector<Type>::PtrVector(std::function<Type*(Type*)> const &copyFunction)
      :
          std::vector<Type*>(),
          d_copyFunction(copyFunction)
  {
  }

  template<typename Type>
  PtrVector<Type>::PtrVector(size_t count, std::function<Type*(Type*)> const &copyFunction)
      :
          std::vector<Type*>(count, 0),
          d_copyFunction(copyFunction)
  {
  }

  template<typename Type>
  template<typename InputIt>
  PtrVector<Type>::PtrVector(InputIt first, InputIt last, std::function<Type*(Type*)> const &copyFunction)
      :
          std::vector<Type*>(first, last),
          d_copyFunction(copyFunction)
  {
  }

  template<typename Type>
  PtrVector<Type>::PtrVector(PtrVector const &other)
      :
          std::vector<Type*>(other),
          d_copyFunction(other.d_copyFunction)
  {
    for(size_t idx = 0; idx != other.size(); ++idx)
      (*this)[idx] = d_copyFunction(other[idx]);
  }

  template<typename Type>
  PtrVector<Type>::PtrVector(PtrVector &&other)
      :
          std::vector<Type*>(std::move(other)),
          d_copyFunction(other.d_copyFunction)
  {
  }

  template<typename Type>
  PtrVector<Type> &PtrVector<Type>::operator=(PtrVector const &other)
  {
    PtrVector<Type> tmp(other);
    this->swap(tmp);
    d_copyFunction.swap(tmp.d_copyFunction);
    return *this;
  }

  template<typename Type>
  PtrVector<Type> &PtrVector<Type>::operator=(PtrVector &&tmp)
  {
    this->swap(tmp);
    d_copyFunction.swap(tmp.d_copyFunction);
    return *this;
  }

  template<typename Type>
  PtrVector<Type>::~PtrVector()
  {
    for(size_t idx = 0; idx != std::vector<Type*>::size(); ++idx)
      delete std::vector<Type*>::at(idx);
  }
} /* namespace dim */
#endif /* PTRVECTOR_HPP_ */
