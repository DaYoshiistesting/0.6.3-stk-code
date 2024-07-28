//  $Id: grand_prix_ending.hpp 2275 2008-09-18 03:24:19Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 Eduardo Hernandez Munoz
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

#ifndef HEADER_GRAND_PRIX_ENDING_H
#define HEADER_GRAND_PRIX_ENDING_H

#include "base_gui.hpp"

class ssgBranch;
class ssgTransform;
class ssgContext;
class SFXBase;

class GrandPrixEnd: public BaseGUI
{
private:
    ssgContext   *m_context;
    ssgTransform *m_kart;
    int           m_current_kart;
    int           m_kart_name_label;
    char         *m_score;
    float         m_clock;
    SFXBase      *m_winner_sound;


public:
         GrandPrixEnd();
        ~GrandPrixEnd();
    void update(float dt);
    void select();
};

#endif
