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

#include "dim/dim.hpp"

namespace dim
{
  class Timer
  {
      std::time_t d_time;
      bool d_running;
      
    public:
      Timer();
      
      void start();
      void stop();
     
      std::time_t const &elapsedTime() const; 
  }
  
  class GPUTimer
  {
      std::shared_ptr d_id;
      std::time_t d_time;
      bool d_running;
  
    public:
      GPUTimer();
      
      GPUTimer(GPUTimer const &other) = delete;
      GPUTimer(GPUTimer &&tmp) = default;
      
      GPUTimer &operator=(GPUTimer const &other) = delete;
      GPUTimer &operator=(GPUTimer &&tmp) = default;
      
      void start();
      void stop();
      
      std::time_t const &elapsedTime() const; 
  }
} /* namespace dim */
#endif /* TIMER_HPP_ */
