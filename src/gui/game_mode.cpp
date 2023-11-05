//  $Id: game_mode.cpp 3034 2009-01-23 05:23:22Z hikerstk $
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 SuperTuxKart-Team
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

#include "game_mode.hpp"

#include "race_manager.hpp"
#include "material_manager.hpp"
#include "user_config.hpp"
#include "challenges/unlock_manager.hpp"
#include "gui/menu_manager.hpp"
#include "gui/widget_manager.hpp"
#include "network/network_manager.hpp"
#include "utils/translation.hpp"

enum WidgetTokens
{
    WTOK_TITLE_SINGLE,
    WTOK_QUICK_RACE_SINGLE,
    WTOK_TIMETRIAL_SINGLE,
    WTOK_FOLLOW_LEADER_SINGLE,
    WTOK_3_STRIKES_SINGLE,
    
    WTOK_TITLE_GP, 
    WTOK_QUICK_RACE_GP,
    WTOK_TIMETRIAL_GP,
    WTOK_FOLLOW_LEADER_GP,
    
    WTOK_HELP,
    WTOK_QUIT
};

GameMode::GameMode()
{
    const int HEIGHT = 7;
    const int WIDTH  = 5;
    
    const WidgetDirection column_1_dir = WGT_DIR_FROM_LEFT;
    const float column_1_loc = 0.1f;
    
    const WidgetDirection column_2_dir = WGT_DIR_FROM_RIGHT;
    const float column_2_loc = 0.1f;
    
    // First the single race events
    // ============================
    Widget *w=widget_manager->addTextWgt(WTOK_TITLE_SINGLE, WIDTH,
                                         HEIGHT, _("Single Race"));
    widget_manager->hideWgtRect(WTOK_TITLE_SINGLE);
    w->setPosition(column_1_dir, column_1_loc, NULL,
                   WGT_DIR_FROM_TOP,  0.2f, NULL);
    Widget *w_prev=w;
    w=widget_manager->addTextButtonWgt(WTOK_QUICK_RACE_SINGLE, WIDTH, HEIGHT,
                                       _("Quick Race"));
    w->setPosition(column_1_dir, column_1_loc, NULL,
                   WGT_DIR_UNDER_WIDGET,  0.0f, w_prev);
    w_prev=w;
    w=widget_manager->addTextButtonWgt(WTOK_TIMETRIAL_SINGLE, WIDTH, HEIGHT,
                                       _("Time Trial"));
    w->setPosition(column_1_dir, column_1_loc, NULL,
                   WGT_DIR_UNDER_WIDGET,  0.0f, w_prev);
    w_prev=w;
    w=widget_manager->addTextButtonWgt(WTOK_FOLLOW_LEADER_SINGLE, WIDTH, HEIGHT,
                                       _("Follow the Leader"));
    w->setPosition(column_1_dir, column_1_loc, NULL,
                   WGT_DIR_UNDER_WIDGET,  0.0f, w_prev);
    w_prev=w;
    
    // don't activate battle mode in single-player mode
    if(race_manager->getNumLocalPlayers()>1 ||
       network_manager->getMode()!=NetworkManager::NW_NONE)
    {
        w=widget_manager->addTextButtonWgt(WTOK_3_STRIKES_SINGLE, WIDTH, HEIGHT,
                                           _("3 Strikes Battle"));
        w->setPosition(column_1_dir, column_1_loc, NULL,
                       WGT_DIR_UNDER_WIDGET,  0.0f, w_prev);
        w_prev=w;
        widget_manager->sameWidth(WTOK_TITLE_SINGLE, WTOK_3_STRIKES_SINGLE);
    }
    else
        widget_manager->sameWidth(WTOK_TITLE_SINGLE, WTOK_FOLLOW_LEADER_SINGLE);
    
    // Then the GPs
    // ============
    w=widget_manager->addTextWgt(WTOK_TITLE_GP, WIDTH,
                                 HEIGHT, _("Grand Prix"));
    widget_manager->hideWgtRect(WTOK_TITLE_GP);
    w->setPosition(column_2_dir, column_2_loc, NULL,
                   WGT_DIR_FROM_TOP,  0.2f, NULL);
    w_prev=w;
    w=widget_manager->addTextButtonWgt(WTOK_QUICK_RACE_GP, WIDTH, HEIGHT,
                                       _("Quick Race"));
    w->setPosition(column_2_dir, column_2_loc, NULL,
                   WGT_DIR_UNDER_WIDGET,  0.0f, w_prev);
    w_prev=w;
    w=widget_manager->addTextButtonWgt(WTOK_TIMETRIAL_GP, WIDTH, HEIGHT,
                                       _("Time Trial"));
    w->setPosition(column_2_dir, column_2_loc, NULL,
                   WGT_DIR_UNDER_WIDGET, 0.0f, w_prev);
    w_prev=w;
    w=widget_manager->addTextButtonWgt(WTOK_FOLLOW_LEADER_GP, WIDTH, HEIGHT,
                                       _("Follow the Leader"));
    w->setPosition(column_2_dir, column_2_loc, NULL,
                   WGT_DIR_UNDER_WIDGET,  0.0f, w_prev);
    w_prev=w;
    widget_manager->sameWidth(WTOK_TITLE_GP, WTOK_FOLLOW_LEADER_GP);

    if(unlock_manager->isLocked("grandprix"))
    {
        for(int i=WTOK_QUICK_RACE_GP; i<=WTOK_FOLLOW_LEADER_GP; i++)
        {
            widget_manager->hideWgtText(i);
            widget_manager->deactivateWgt(i);

            widget_manager->setWgtColor(i, WGT_WHITE);
            widget_manager->setWgtLockTexture(i);
            widget_manager->showWgtTexture(i);
        }
    }

    if(unlock_manager->isLocked("followtheleader"))
    {
        widget_manager->hideWgtText(WTOK_FOLLOW_LEADER_SINGLE);
        widget_manager->deactivateWgt(WTOK_FOLLOW_LEADER_SINGLE);
        widget_manager->setWgtLockTexture(WTOK_FOLLOW_LEADER_SINGLE);
        widget_manager->showWgtTexture(WTOK_FOLLOW_LEADER_SINGLE);
        //widget_manager->setWgtText(WTOK_FOLLOW_LEADER_SINGLE,
        //                                _("Fulfil challenge to unlock"));
        widget_manager->hideWgtText(WTOK_FOLLOW_LEADER_GP);
        widget_manager->deactivateWgt(WTOK_FOLLOW_LEADER_GP);
        widget_manager->setWgtLockTexture(WTOK_FOLLOW_LEADER_GP);
        widget_manager->showWgtTexture(WTOK_FOLLOW_LEADER_GP);
        //widget_manager->setWgtText(WTOK_FOLLOW_LEADER_GP,
        //                                _("Fulfil challenge to unlock"));
    }


    // help
    w=widget_manager->addTextButtonWgt( WTOK_HELP, WIDTH, HEIGHT, _("Game mode help"));
    widget_manager->setWgtTextSize( WTOK_HELP, WGT_FNT_SML );
    w->setPosition(WGT_DIR_CENTER, 0.0f, NULL, WGT_DIR_UNDER_WIDGET, 0.1f, w_prev);
    w_prev=w;

    // return button
    w=widget_manager->addTextButtonWgt(WTOK_QUIT, WIDTH, HEIGHT, _("Press <ESC> to go back"));
    widget_manager->setWgtTextSize( WTOK_QUIT, WGT_FNT_SML );
    w->setPosition(WGT_DIR_CENTER, 0.0f, NULL, WGT_DIR_FROM_BOTTOM, 0.0f, w_prev);

    widget_manager->layout(WGT_AREA_ALL);
}

//-----------------------------------------------------------------------------
GameMode::~GameMode()
{
    widget_manager->reset();
}

//-----------------------------------------------------------------------------
void GameMode::select()
{
    int mode = widget_manager->getSelectedWgt();
    switch (mode)
    {
    case WTOK_QUICK_RACE_SINGLE:
         race_manager->setMajorMode(RaceManager::MAJOR_MODE_SINGLE);
         race_manager->setMinorMode(RaceManager::MINOR_MODE_QUICK_RACE);
         break;
    case WTOK_TIMETRIAL_SINGLE:
         race_manager->setMajorMode(RaceManager::MAJOR_MODE_SINGLE);
         race_manager->setMinorMode(RaceManager::MINOR_MODE_TIME_TRIAL);
         break;
    case WTOK_FOLLOW_LEADER_SINGLE:
         race_manager->setMajorMode(RaceManager::MAJOR_MODE_SINGLE);
         race_manager->setMinorMode(RaceManager::MINOR_MODE_FOLLOW_LEADER);
         break;
    case WTOK_3_STRIKES_SINGLE:
        race_manager->setMajorMode(RaceManager::MAJOR_MODE_SINGLE);
        race_manager->setMinorMode(RaceManager::MINOR_MODE_3_STRIKES);
        break;
    case WTOK_QUICK_RACE_GP:
         race_manager->setMajorMode(RaceManager::MAJOR_MODE_GRAND_PRIX);
         race_manager->setMinorMode(RaceManager::MINOR_MODE_QUICK_RACE);
         break;
    case WTOK_TIMETRIAL_GP:
         race_manager->setMajorMode(RaceManager::MAJOR_MODE_GRAND_PRIX);
         race_manager->setMinorMode(RaceManager::MINOR_MODE_TIME_TRIAL);
         break;
    case WTOK_FOLLOW_LEADER_GP:
         race_manager->setMajorMode(RaceManager::MAJOR_MODE_GRAND_PRIX);
         race_manager->setMinorMode(RaceManager::MINOR_MODE_FOLLOW_LEADER);
         break;
    case WTOK_HELP:
         menu_manager->pushMenu(MENUID_HELP3);
         return;
    case WTOK_QUIT:
         menu_manager->popMenu();
        return;
    default: break;
    }
    menu_manager->pushMenu(MENUID_CHARSEL_P1);
}



