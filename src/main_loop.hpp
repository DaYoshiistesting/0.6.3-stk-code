//  $Id: main_loop.hpp 855 2006-11-17 01:50:37Z coz $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_MAIN_LOOP_HPP
#define HEADER_MAIN_LOOP_HPP
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#else
#  ifdef WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#  endif
#  include <GL/gl.h>
#endif

#include <SDL/SDL_types.h>

/** Management class for the whole gameflow, this is where the
    main-loop is */
class MainLoop
{
private:
    bool m_abort;

    int      m_frame_count;
    Uint32   m_curr_time;
    Uint32   m_prev_time;
    GLuint   m_title_screen_texture;
    GLuint   m_bg_texture;

public:
         MainLoop();
        ~MainLoop();
    void run();
    void abort();
    void loadBackgroundImages();
};   // MainLoop

extern MainLoop* main_loop;

#endif

/* EOF */
