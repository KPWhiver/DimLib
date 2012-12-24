// tools.hpp
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

#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <unordered_map>
#include <tuple>
#include <functional>

#include "dim/core/dim.hpp"

namespace dim
{
  /*
   * Draws a quad that covers the entire screen
   */
  void drawFullscreenQuad();

  /*
   * Turns a GLuint into a type
   */
  template<GLuint Index>
  struct Idx
  {
    static const GLuint idx = Index;
  };

  /*
   * Turns a GLuint into a type
   */
  template<GLuint Index>
  struct State
  {
    static const GLuint state = Index;
  };

  /*
   * Calls a function only if it's arguments are different compared to the last time it's called
   */
  template<typename ...Types>
  void changeState(void (*function)(Types...), Types... params)
  {
    static std::unordered_map<void (*)(Types...), std::tuple<Types...>> map;

    std::tuple<Types...> arguments(params...);

    // first time it's called
    if(map.find(function) == map.end())
      map[function] = arguments;

    if(map[function] != arguments)
    {
      function(params...);
      map[function] = arguments;
    }
  }

  /*
   * Calls a function with Index only if it's arguments are different compared to the last time it's called
   */
  template<typename Index, typename ...Types>
  void changeStateForIdx(void (*function)(Types...), GLuint idx, Types... params)
  {
    static std::unordered_map<void (*)(Types...), std::tuple<Types...>> map;

    std::tuple<Types...> arguments(params...);

    // first time it's called
    if(map.find(function) == map.end())
      map[function] = arguments;

    if(map[function] != arguments)
    {
      function(Index::idx, params...);
      map[function] = arguments;
    }
  }

  /*
   * Changes a state only if it's on/off status is about to be changed
   */
  template<typename StateT>
  void toggleState(void (*function)(GLuint param))
  {
    static void (*status)(GLuint param) = 0;

    // first time it's called
    if(status == 0)
      status = function;

    if(status != function)
    {
      function(StateT::state);
      status = function;
    }
  };

  /*
   * Changes a state with Index only if it's on/off status is about to be changed
   */
  template<typename StateT, typename Index>
  void toggleStateForIdx(void (*function)(GLuint param, GLuint idx))
  {
    static void (*status)(GLuint param, GLuint idx) = 0;

    // first time it's called
    if(status == 0)
      status = function;

    if(status != function)
    {
      function(StateT::state, Index::idx);
      status = function;
    }
  };
} /* namespace dim */
#endif /* TOOLS_HPP_ */
