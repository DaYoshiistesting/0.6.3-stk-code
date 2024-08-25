//  $Id: item.hpp 2405 2008-10-31 20:48:19Z auria $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Steve Baker <sjbaker1@airmail.net>
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

#ifndef HEADER_BUBBLEGUMITEM_H
#define HEADER_BUBBLEGUMITEM_H

#include "items/item.hpp"

// -----------------------------------------------------------------------------
class BubbleGumItem : public Item
{
public:
                 BubbleGumItem(ItemType type, const Vec3& xyz, 
                               const Vec3 &normal, ssgEntity* model,
                               unsigned int item_id);
                ~BubbleGumItem ();
    virtual void isCollected(float t);
}
;   // class Item

#endif
