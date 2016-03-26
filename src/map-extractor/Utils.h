/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2016  MaNGOS project <http://getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include "TypeDefines.h"
#include <string>

/// Core Numbers
#define CLIENT_CLASSIC  0
#define CLIENT_TBC      1
#define CLIENT_WOTLK    2
#define CLIENT_CATA     3
#define CLIENT_MOP      4
#define CLIENT_WOD      5
#define CLIENT_LEGION   6

bool GetCoreAndBuildNumber(uint32& build, int8& core);
void ShowBanner(const char* title, int iCoreNumber);
void ShowWebsiteBanner();
void SetMapMagicVersion(int iCoreNumber, char* magic);
void SetVMapMagicVersion(int iCoreNumber, char* magic);
void CreateDir(const char* path);
bool FileExist(const char* sFileName);
bool IsTransportMap(int mapID);
bool ShouldSkipMap(int mapID, bool m_skipContinents, bool m_skipJunkMaps, bool m_skipBattlegrounds);

#endif