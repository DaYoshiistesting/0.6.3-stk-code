//  $Id: attachment.cpp 3029 2009-01-22 22:27:13Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 Joerg Henrichs
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

#define _WINSOCKAPI_
#include <plib/ssg.h>

#include "stk_config.hpp"
#include "user_config.hpp"
#include "items/attachment.hpp"
#include "items/attachment_manager.hpp"
#include "items/projectile_manager.hpp"
#include "karts/kart.hpp"
#include "network/race_state.hpp"
#include "network/network_manager.hpp"
#include "utils/constants.hpp"

Attachment::Attachment(Kart* _kart)
{
    m_type           = ATTACH_NOTHING;
    m_time_left      = 0.0;
    m_kart           = _kart;
    m_holder         = new ssgSelector();
    m_previous_owner = NULL;
    m_kart->getModelTransform()->addKid(m_holder);

    for(int i=ATTACH_FIRST; i<ATTACH_MAX; i++)
    {
        ssgEntity *p=attachment_manager->getModel((attachmentType)i);
        m_holder->addKid(p);
    }
    m_holder->select(0);
}   // Attachment

//-----------------------------------------------------------------------------
Attachment::~Attachment()
{
    ssgDeRefDelete(m_holder);
}   // ~Attachment

//-----------------------------------------------------------------------------
void Attachment::set(attachmentType type, float time, Kart *current_kart)
{
    clear();
    m_holder->selectStep(type);
    m_type           = type;
    m_time_left      = time;
    m_previous_owner = current_kart;
    // A parachute can be attached as result of the usage of an item. In this
    // case we have to save the current kart speed so that it can be detached
    // by slowing down.
    if(m_type==ATTACH_PARACHUTE)
    {
        m_initial_speed = m_kart->getSpeed();
        if(m_initial_speed <= 1.5) m_initial_speed = 1.5; // if going very slowly or backwards, braking won't remove parachute
    }
}   // set

// -----------------------------------------------------------------------------
/** Removes any attachement currently on the kart. As for the anvil attachment,
 *  takes care of resetting the owner kart's physics structures to account for
 *  the updated mass.
 */
void Attachment::clear()
{
    m_type=ATTACH_NOTHING; 
    m_time_left=0.0;
    m_holder->select(0);

    // Resets the weight of the kart if the previous attachment affected it 
    // (e.g. anvil). This must be done *after* setting m_type to
    // ATTACH_NOTHING in order to reset the physics parameters.
    m_kart->updatedWeight();
}   // clear

// -----------------------------------------------------------------------------
void Attachment::hitBanana(const Item &item, int new_attachment)
{
    if(user_config->m_profile) return;
    float leftover_time   = 0.0f;
    
    switch(getType())   // If there already is an attachment, make it worse :)
    {
    case ATTACH_BOMB:
        projectile_manager->newExplosion(m_kart->getXYZ());
        m_kart->handleExplosion(m_kart->getXYZ(), /*direct_hit*/ true);
        clear();
        if(new_attachment==-1) 
            new_attachment = m_random.get(3);
        break;
    case ATTACH_ANVIL:
        // if the kart already has an anvil, attach a new anvil, 
        // and increase the overall time 
        new_attachment = 2;
        leftover_time     = m_time_left;
        break;
    case ATTACH_PARACHUTE:
        new_attachment = 2;  // anvil
        leftover_time     = m_time_left;
        break;
    default:
        if(new_attachment==-1)
            new_attachment = m_random.get(3);
    }   // switch

    // Save the information about the attachment in the race state
    // so that the clients can be updated.
    if(network_manager->getMode()==NetworkManager::NW_SERVER)
    {
        race_state->itemCollected(m_kart->getWorldKartId(),
                                  item.getItemId(),
                                  new_attachment);
    }

    switch (new_attachment)
    {
    case 0: 
        set( ATTACH_PARACHUTE,stk_config->m_parachute_time+leftover_time);
        m_initial_speed = m_kart->getSpeed();
        if(m_initial_speed <= 1.5) m_initial_speed = 1.5; // if going very slowly or backwards, braking won't remove parachute
        // if ( m_kart == m_kart[0] )
        //   sound -> playSfx ( SOUND_SHOOMF ) ;
        break ;
    case 1:
        set( ATTACH_BOMB, stk_config->m_bomb_time+leftover_time);
        // if ( m_kart == m_kart[0] )
        //   sound -> playSfx ( SOUND_SHOOMF ) ;
        break ;
    case 2:
        set( ATTACH_ANVIL, stk_config->m_anvil_time+leftover_time);
        // if ( m_kart == m_kart[0] )
        //   sound -> playSfx ( SOUND_SHOOMF ) ;
        // Reduce speed once (see description above), all other changes are
        // handled in Kart::updatePhysics
        m_kart->adjustSpeed(stk_config->m_anvil_speed_factor);
        m_kart->updatedWeight();
        break ;
    }   // switch 
}   // hitBanana

//-----------------------------------------------------------------------------
//** Moves a bomb from kart FROM to kart TO.
void Attachment::moveBombFromTo(Kart *from, Kart *to)
{
    to->setAttachmentType(ATTACH_BOMB,
                          from->getAttachment()->getTimeLeft()+
                          stk_config->m_bomb_time_increase, from);
    from->getAttachment()->clear();
}   // moveBombFromTo

//-----------------------------------------------------------------------------
void Attachment::update(float dt)
{
    if(m_type==ATTACH_NOTHING) return;
    m_time_left -=dt;

    switch (m_type)
    {
    case ATTACH_PARACHUTE:
        // Partly handled in Kart::updatePhysics
        // Otherwise: disable if a certain percantage of
        // initial speed was lost
        if(m_kart->getSpeed() <= m_initial_speed*stk_config->m_parachute_done_fraction)
        {
            m_time_left = -1;
        }
        break;
    case ATTACH_ANVIL:     // handled in Kart::updatePhysics
    case ATTACH_NOTHING:   // Nothing to do, but complete all cases for switch
    case ATTACH_MAX:
        break;
    case ATTACH_BOMB:
        if(m_time_left<=0.0) 
        {
            projectile_manager->newExplosion(m_kart->getXYZ());
            m_kart->handleExplosion(m_kart->getXYZ(), 
                                    /*direct_hit*/ true);
        }
        break;
    case ATTACH_TINYTUX:
        if(m_time_left<=0.0) m_kart->endRescue();
        break;
    }   // switch

    // Detach attachment if its time is up.
    if ( m_time_left <= 0.0f)
        clear();
}   // update
//-----------------------------------------------------------------------------
