// sortedvector.hpp
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

#ifndef SORTEDVECTOR_HPP
#define SORTEDVECTOR_HPP

#include <vector>
#include <functional>

namespace dim
{
  template <typename Type>
  class SortedVector : public std::vector<Type>
  {
    public:
      explicit SortedVector(size_t count);

      template<typename InputIt>
      SortedVector(InputIt first, InputIt last);

      SortedVector(SortedVector &&other) = default;


      iterator insert(const_iterator pos, const T& value) = delete;
      iterator insert(const_iterator pos, T&& value) = delete;
      iterator insert(const_iterator pos, size_type count, const T& value) = delete;

      template< class InputIt >
      iterator insert(const_iterator pos, InputIt first, InputIt last) = delete;
      iterator insert(const_iterator pos, std::initializer_list<T> ilist) = delete;

      template< class... Args >
      iterator emplace( const_iterator pos, Args&&... args ) = delete;

      template< class... Args >
      void emplace_back( Args&&... args ) = delete;

      void resize( size_type count );
      void resize( size_type count, const value_type& value);

      void push_back( const T& value );
      void push_back( T&& value );
  };

  template<typename Type>
  SortedVector<Type>::SortedVector(size_t count)
      :
          std::vector<Type*>(count)
  {
  }

  template<typename Type>
  template<typename InputIt>
  SortedVector<Type>::SortedVector(InputIt first, InputIt last)
      :
          std::vector<Type*>()
  {
    for(auto iter = first; iter != last; ++iter)
      push_back(*iter);
  }

  template<typename Type>
  void SortedVector<Type>::resize(size_type count)
  {
    static Type instance;
    resize(count, instance);
  }

  template<typename Type>
  void SortedVector<Type>::resize(size_type count, value_type const &value)
  {
    if(count > size())
    {
      size_t difference = size() - count;

      for(size_t idx = 0; idx != difference; ++idx)
        push_back(value);
    }
    else
      vector<Type>::resize(count);
  }

  template<typename Type>
  void SortedVector<Type>::push_back(T const &value)
  {
    auto iter = std::lower_bound(begin(), end(), value);

    vector<Type>::insert(iter, value);
  }

  template<typename Type>
  void SortedVector<Type>::push_back(T const &&value)
  {
    auto iter = std::lower_bound(begin(), end(), value);

    vector<Type>::insert(iter, std::move(value));
  }
} /* namespace dim */
#endif /* SORTEDVECTOR_HPP_ */
