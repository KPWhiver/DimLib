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
  Timer::Timer(bool timeGPU)
    :
      d_id(0),
      d_CPUtime(0),
      d_startCPUtime(chrono::system_clock::now()),
      d_GPUtime(0),
      d_running(false)
  {
    if(timeGPU && GLEW_ARB_timer_query)
      glGenQueries(1, &d_id);
  }
  
  Timer::~Timer()
  {
    if(d_id != 0)
      glDeleteQueries(1, &d_id);
  }

  void Timer::start()
  {
    d_running = true;
    if(d_id != 0)
      glBeginQuery(GL_TIME_ELAPSED, d_id);
  }
  
  void Timer::stop()
  {
    if(d_running == true)
    {
      d_CPUtime = d_startCPUtime - chrono::system_clock::now();

      if(d_id != 0)
        glEndQuery(GL_TIME_ELAPSED);

      d_running = false;
    }
  }
      
  Timer::milliseconds const &Timer::elapsedCPUtime()
  {
    if(d_running == true)
    {
      d_CPUtime = d_startCPUtime - chrono::system_clock::now();
    }

    return d_CPUtime;
  }

  Timer::milliseconds const &Timer::elapsedGPUtime()
  {
    if(d_running == true)
    {
      log(__FILE__, __LINE__, LogType::note, "Calling Timer::elapsedGPUtime() before calling Timer::stop() will result in the time before starting");
      return d_GPUtime;
    }

    if(GLEW_ARB_timer_query)
      log(__FILE__, __LINE__, LogType::error, "GPUtime measurement not supported on this GPU");
      
    if(d_id != 0)
    {
      GLuint64 time;
      
      glGetQueryObjectui64v(d_id, GL_QUERY_RESULT, &time);
    
      d_GPUtime = milliseconds(time / 1000000.0);
    }
      
    return d_GPUtime;
  }

  bool GPUtimeSupport()
  {
    return GLEW_ARB_timer_query;
  }
}
