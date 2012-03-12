// mouse.hpp
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

#ifndef MOUSE_HPP
#define MOUSE_HPP

#include "DIM/dim.hpp"

namespace dim
{

	class Mouse
	{
	  bool d_lButton;
	  bool d_rButton;
	  
	  bool d_lPressed;
	  bool d_rPressed;
	  
	  bool d_lRelease;
	  bool d_rRelease;
	  
	  glm::ivec2 d_coor;

  public:  
    Mouse();
    
    void update();

    bool lPressed() const;
    bool rPressed() const;
    
    bool lRelease() const;
    bool rRelease() const;
    
    glm::ivec2 const &coor() const;

  private:  
    //void rRelease();
    //void lRelease();

	};

}

#endif /* BUTTON_HPP_ */
