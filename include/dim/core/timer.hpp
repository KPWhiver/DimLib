// timer.hpp
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

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include "dim/core/dim.hpp"

#include <chrono>

namespace dim
{
  class Timer
  {
      typedef std::chrono::duration<double, std::milli> milliseconds;

      GLuint d_id;
      milliseconds d_CPUtime;
      std::chrono::time_point<std::chrono::steady_clock, milliseconds> d_startCPUtime;
      milliseconds d_GPUtime;
      bool d_running;
  
    public:
      Timer(bool timeGPU = true);
      
      Timer(Timer const &other) = delete;
      Timer(Timer &&tmp) = default;
      
      ~Timer();

      Timer &operator=(Timer const &other) = delete;
      Timer &operator=(Timer &&tmp) = default;
      
      void start();
      void stop();
      
      milliseconds const &elapsedCPUtime();
      milliseconds const &elapsedGPUtime();

      static bool GPUtimeSupport();
  };
} /* namespace dim */
#endif /* TIMER_HPP_ */
