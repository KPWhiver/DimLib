// timer.cpp
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

#include "dim/timer.hpp"

using namespace std;

namespace dim
{
  Timer::Timer()
    :
      d_time(),
      d_running(false)
  {
  }
      
  void Timer::start()
  {
    d_running = true;
  }
  
  void Timer::stop()
  {
    d_time = getTime();
    d_running = false;
  }
       
  time_t const &Timer::elapsedTime() const
  {
    if(d_running == true)
      d_time = getTime();  
      
    return d_time;
  }
  
  //********//
  
  GPUTimer::GPUTimer()
    :
      d_id(new GLuint(0), [](GLuint *ptr)
      {
        glDeleteQueries(1, d_id);
        delete ptr;
      }),
      d_time(),
      d_running(false)
  {
    glGenQueries(1, d_id);
  }
  
  void GPUTimer::start()
  {
    d_running = true;
    glBeginQuery(GL_TIME_ELAPSED, *d_id);
  }
  
  void GPUTimer::stop()
  {
    if(d_running == true)
    {
      glEndQuery(GL_TIME_ELAPSED);
      d_running = false;
    }
  }
      
  time_t const &GPUTimer::elapsedTime() const
  {
    if(d_running == true)
      log(__FILE__, __LINE__, LogType::error, "You need to call GPUTimer::stop() before calling GPUTimer::elapsedTime()"); 
      
    GLuint time;
      
    glGetQueryObjectui64(*d_id, GL_QUERY_RESULT, &time);
    
    d_time += time;
      
    return d_time;
  }
}
