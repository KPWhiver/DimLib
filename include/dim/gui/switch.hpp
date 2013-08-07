// switch.hpp
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

#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "dim/core/texture.hpp"
#include "dim/gui/component.hpp"

#include <functional>
#include <string>
#include <memory>

namespace dim
{
  class Switch : public Component
  {
    std::function<void(bool)> d_listenerFunction;

    int d_x, d_y;
    size_t d_width, d_height;
    Texture<GLubyte> d_text;
    std::string d_strText;

    bool d_enabled;
    bool d_selected;

  public:
    Switch(int x, int y, size_t width, size_t height, std::string const &text);
    Switch();
    //~Button();
    virtual void draw(int x, int y);
    virtual bool listen(int x, int y, glm::ivec2 const &mouse);

    void setListener(std::function<void(bool)> const &listenerFunction);
    virtual void setContext(Context *context);
    void enable(bool state);
    bool enabled() const;

  private:
    virtual Component *v_clone() const;
  };

}

#endif /* SWITCH_HPP_ */
