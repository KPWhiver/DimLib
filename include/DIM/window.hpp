// window.hpp
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

#ifndef WINDOW
#define WINDOW

#include <string>

#include "DIM/mouse.hpp"

namespace dim
{

	class Window
	{
		size_t d_height, d_width;
		static bool s_set;

		double d_fps;
		size_t d_maxRunFps;

		Mouse d_mouse;

		double d_startTime;

	public:
		enum Mode
		{
			fullscreen, windowed,
		};

		Window(size_t width, size_t height, Window::Mode mode, std::string title);
		Window(Window const &other) = delete;
		void operator=(Window const &other) = delete;

		size_t height() const;
		size_t width() const;

		double fps() const;
		double fpsFactor() const;

		void setMaxRunFps(size_t fps);

		Mouse const &mouse() const;

    void renderTo();
    void renderToPart(size_t x, size_t y, size_t width, size_t height, bool clear);

		void swapBuffers();

	private:
		Window::Mode d_mode;

		//void set(size_t width, size_t height, Window::mode mode, std::string title);





	};

}

#endif
