//  $Id: feature_unlocked.hpp 1305 2007-11-26 14:28:15Z hiker $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2008 Joerg Henrichs
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

#ifndef HEADER_FEATURE_UNLOCKED_H
#define HEADER_FEATURE_UNLOCKED_H

#include <vector>
#include "challenges/challenge.hpp"
#include "base_gui.hpp"

class FeatureUnlocked: public BaseGUI
{
private:
    std::vector<const Challenge*> m_new_features;

public:
         FeatureUnlocked();
        ~FeatureUnlocked();
    void select       ();
};  // FeatureUnlocked

#endif
