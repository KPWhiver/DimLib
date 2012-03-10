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

namespace dim
{

template <typename Type, size_t Maxfirst>
class Onepair
{
  Type d_data;
  
  public:
    enum Parameter
    {
      half,
      third,
    };

    Onepair(Type first, Type second);
    Onepair();
    
    Type first() const;
    Type second() const;
    
    void setFirst(Type first);
    void setSecond(Type second);

    operator Type() const;
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
Onepair<Type, Maxfirst>::Onepair(Type first, Type second)
{
  d_data = first + second * Maxfirst;
}

template <typename Type, size_t Maxfirst>
Onepair<Type, Maxfirst>::Onepair()
{
  d_data = 0;
}

template <typename Type, size_t Maxfirst>
Type Onepair<Type, Maxfirst>::first() const
{
  d_data % Maxfirst;
}

template <typename Type, size_t Maxfirst>
Type Onepair<Type, Maxfirst>::second() const
{
  d_data / Maxfirst;
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
Onepair<Type, Maxfirst>::operator Type() const
{
  return d_data;
}

}
#endif
