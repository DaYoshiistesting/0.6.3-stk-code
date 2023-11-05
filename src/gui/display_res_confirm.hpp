//  $Id: display_res_confirm.hpp 2128 2008-06-13 00:53:52Z cosmosninja $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2008 Paul Elms
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

#ifndef HEADER_DISPLAY_RES_CONFIRM_H
#define HEADER_DISPLAY_RES_CONFIRM_H

#include "base_gui.hpp"

#include <SDL/SDL.h>

class DisplayResConfirm: public BaseGUI
{
public:
    DisplayResConfirm( const bool FROM_WINDOW_ );
    ~DisplayResConfirm();

    void select();
    void countdown();
    void handle(GameAction ga, int value);
    
private:
    char m_count[60];
    int m_counter;
    const bool FROM_WINDOW;

    SDL_TimerID m_timer;
};

Uint32 timeout(Uint32 interval, void *param);

#endif
