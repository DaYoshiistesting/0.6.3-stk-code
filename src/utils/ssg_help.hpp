//  $Id: ssg_help.hpp 796 2006-09-27 07:06:34Z hiker $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004-2005 Steve Baker <sjbaker1@airmail.net>
//  Copyright (C) 2006 SuperTuxKart-Team, Joerg Henrichs, Steve Baker
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

#ifndef HEADER_SSG_HELP_H
#define HEADER_SSG_HELP_H

#include <plib/ssg.h>
#include "vec3.hpp"

namespace SSGHelp {
    //* Several useful functions which don't fit anywhere else
    void          createDisplayLists(ssgEntity *entity);
    void          print_model       (ssgEntity *entity, const int indent,
                                     const int maxLevel);
    void          MinMax            (const ssgEntity *p,
                                     Vec3 *min, Vec3 *max);
};   // namespace SSGHelp
#endif

/* EOF */

