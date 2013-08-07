// onepair.hpp
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

#ifndef ONEPAIR_HPP
#define ONEPAIR_HPP

#include <functional>

namespace dim
{

template <typename Type, size_t Maxfirst>
class Onepair
{


  Type d_data;
  
  public:
    struct Hash
    {
      constexpr size_t operator()(Onepair<Type, Maxfirst> const &toHash) const
      {
        return std::hash<Type>{}(toHash.value());
      }
    };

    typedef Type StorageType;
    static size_t const maxFirst = Maxfirst;

    enum Parameter
    {
      half,
      third
    };

    constexpr Onepair(Type first, Type second);
    constexpr Onepair();

    bool operator==(Onepair const &other) const;
    
    constexpr Type first() const;
    constexpr Type second() const;
    
    void setFirst(Type first);
    void setSecond(Type second);

    constexpr Type value() const;

    //operator Type() const;
};

//template <typename type, size_t Maxfirst>
//  struct Divide
//  {
//    enum
//    {
//      value = (Maxfirst == Onepair<type, Maxfirst>::half ? std::numeric_limits<type>::max() / 2 :
//              (Maxfirst == Onepair<type, Maxfirst>::third ? std::numeric_limits<type>::max() / 3 :
//               Maxfirst))
//    };
//  };

template <typename Type, size_t Maxfirst>
constexpr Onepair<Type, Maxfirst>::Onepair(Type first, Type second)
:
  d_data(first + second * Maxfirst)
{
}

template <typename Type, size_t Maxfirst>
constexpr Onepair<Type, Maxfirst>::Onepair()
:
  d_data(0)
{
}

template <typename Type, size_t Maxfirst>
constexpr Type Onepair<Type, Maxfirst>::first() const
{
  return d_data % Maxfirst;
}

template <typename Type, size_t Maxfirst>
constexpr Type Onepair<Type, Maxfirst>::second() const
{
  return d_data / Maxfirst;
}

template <typename Type, size_t Maxfirst>
void Onepair<Type, Maxfirst>::setFirst(Type first)
{
  d_data = first + (d_data / Maxfirst) * Maxfirst;
}

template <typename Type, size_t Maxfirst>
void Onepair<Type, Maxfirst>::setSecond(Type second)
{
  d_data = (d_data % Maxfirst) + second * Maxfirst;
}

template <typename Type, size_t Maxfirst>
bool Onepair<Type, Maxfirst>::operator==(Onepair const &other) const
{
  return d_data == other.d_data;
}

template <typename Type, size_t Maxfirst>
constexpr Type Onepair<Type, Maxfirst>::value() const
{
  return d_data;
}

/*template <typename Type, size_t Maxfirst>
Onepair<Type, Maxfirst>::operator Type() const
{
  return d_data;
}*/

}
#endif
