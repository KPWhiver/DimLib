// menu.hpp
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

#ifndef MENU_HPP
#define MENU_HPP

#include "dim/core/texture.hpp"
#include "dim/gui/menuitem.hpp"
#include "dim/gui/component.hpp"
#include "dim/util/ptrvector.hpp"

#include <memory>

namespace dim
{
  class Menu : public Component
  {
    size_t d_width, d_height;
    int d_x, d_y;

    dim::PtrVector<MenuItem> d_items;
    bool d_active;

    static Menu* s_active;

  public:
    Menu(size_t d_width, size_t d_height);
    Menu();
    ~Menu();

    void draw(int x, int y) override;
    bool listen(int x, int y, glm::ivec2 const &mouse, Event leftEvent, Event rightEvent) override;
    void add(MenuItem *item);
    void activate(int x, int y);
    void deactivate();
    bool active() const;
    virtual void setContext(Context *context);

  private:
    virtual Component *v_clone() const;
};

}

#endif /* BUTTON_HPP_ */
