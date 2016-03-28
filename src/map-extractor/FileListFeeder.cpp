/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2016  MaNGOS project <https://getmangos.eu>
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

#include "FileListFeeder.h"

FileListFeeder::FileListFeeder() : m_listLocale(), m_curLocale(-1)
{
    m_list.resize(FLT_NUM);
    FileList eList;
    for (uint8 i = FLT_BASE; i < FLT_NUM; ++i)
        m_list[i] = eList;

    m_listLocale.push_back("enGB");
    m_listLocale.push_back("enUS");
    m_listLocale.push_back("deDE");
    m_listLocale.push_back("esES");
    m_listLocale.push_back("frFR");
    m_listLocale.push_back("koKR");
    m_listLocale.push_back("zhCN");
    m_listLocale.push_back("zhTW");
    m_listLocale.push_back("enCN");
    m_listLocale.push_back("enTW");
    m_listLocale.push_back("esMX");
    m_listLocale.push_back("ruRU");
}

FileListFeeder::~FileListFeeder()
{
    m_list.clear();
    m_listLocale.clear();
}

FileList const& FileListFeeder::GetFileList(FileListType idx)
{
    return m_list[idx];
}

bool FileListFeeder::Init(char const *m_path, uint32 m_build, int8 m_core)
{
    bool result = true;
    std::string tempLocDir;
    std::string dataPath = std::string(m_path) + "/Data/";

    if (m_core != CLIENT_CLASSIC)
    {
        std::printf(" Detecting locale...");

        int8 i = 0;
        for (std::vector<std::string>::const_iterator it = m_listLocale.begin(); it != m_listLocale.end(); ++it, i++)
        {
            tempLocDir = dataPath + (*it) + "/locale-" + (*it) + ".MPQ";
            if (FileExist(tempLocDir.c_str()))
            {
                printf("%s\n", it->c_str());
                m_curLocale = i;
                break;
            }
        }
        if (m_curLocale < 0)
        {
            printf(" Couldn't find any locale\n");
            return false;
        }
    }
    else
      { m_curLocale = 0; }

    switch (m_core)
    {
        case CLIENT_CLASSIC:
        {
            // base files
            m_list[FLT_BASE].push_back(dataPath + "wmo.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "terrain.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "model.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "dbc.MPQ");

            dataPath += "patch";
            scan_patches(dataPath, FLT_BASE);
        }
        break;

        case CLIENT_TBC:
        case CLIENT_WOTLK:
        {
            // base files
            m_list[FLT_BASE].push_back(dataPath + "common.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "expansion.MPQ");

            if (m_core == CLIENT_WOTLK)
            {
                m_list[FLT_BASE].push_back(dataPath + "common-2.MPQ");
                m_list[FLT_BASE].push_back(dataPath + "lichking.MPQ");
            }

            tempLocDir = dataPath + "patch";
            scan_patches(tempLocDir, FLT_BASE);

            // locale files
            tempLocDir = dataPath + m_listLocale[m_curLocale] + "/";
            m_list[FLT_LOCALE].push_back(tempLocDir + "locale-" + m_listLocale[m_curLocale] + ".MPQ");

            tempLocDir = tempLocDir + "patch-" + m_listLocale[m_curLocale];
            scan_patches(tempLocDir, FLT_LOCALE);
        }
        break;

        case CLIENT_CATA:
        {
            //TODO implement an algorithms here. for now we'll add by hand, for 4.4.3 (build 15595)

            // base files
            m_list[FLT_BASE].push_back(dataPath + "art.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "expansion1.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "expansion2.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "expansion3.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "world.MPQ");
            m_list[FLT_BASE].push_back(dataPath + "world2.MPQ");

            // base patches
            m_list[FLT_BASE_PATCH].push_back(dataPath + "wow-update-base-15211.MPQ");
            m_list[FLT_BASE_PATCH].push_back(dataPath + "wow-update-base-15354.MPQ");
            m_list[FLT_BASE_PATCH].push_back(dataPath + "wow-update-base-15595.MPQ");

            // locale files
            tempLocDir = dataPath + m_listLocale[m_curLocale] + "/";
            m_list[FLT_LOCALE].push_back(tempLocDir + "locale-" + m_listLocale[m_curLocale] + ".MPQ");

            // locale patches
            tempLocDir = dataPath + m_listLocale[m_curLocale] + "/";
            m_list[FLT_LOCALE_PATCH].push_back(tempLocDir + "wow-update-" + m_listLocale[m_curLocale] + "-15211.MPQ");
            m_list[FLT_LOCALE_PATCH].push_back(tempLocDir + "wow-update-" + m_listLocale[m_curLocale] + "-15354.MPQ");
            m_list[FLT_LOCALE_PATCH].push_back(tempLocDir + "wow-update-" + m_listLocale[m_curLocale] + "-15595.MPQ");
        }
        break;

        default:
            break;
    }
    if (result)
    {
        std::printf("\n BASE files:\n");
        std::printf(" ---------------------\n");
        list_files(FLT_BASE);
        if (m_core != CLIENT_CLASSIC)
        {
            std::printf("\n LOCALE files:\n");
            std::printf(" ---------------------\n");
            list_files(FLT_LOCALE);
        }
        if (m_core == CLIENT_CATA)
        {
            std::printf("\n BASE_PATCH files:\n");
            std::printf(" ---------------------\n");
            list_files(FLT_BASE_PATCH);
            std::printf("\n LOCALE_PATCH files:\n");
            std::printf(" ---------------------\n");
            list_files(FLT_LOCALE_PATCH);
        }
    }
    return result;
}

void FileListFeeder::scan_patches(std::string& prefix, FileListType index)
{
    int i;
    char path[1024];

    for (i = 1; i <= 8; i++)
    {
        if (i != 1)
          std::sprintf(path, "%s-%d.MPQ", prefix.c_str(), i);
        else
          sprintf(path, "%s.MPQ",prefix.c_str());

        if (FileExist(path))
          m_list[index].push_back(path);
    }
}

void FileListFeeder::list_files(FileListType idx)
{
    for (FileList::const_iterator it = m_list[idx].begin(); it != m_list[idx].end(); ++it)
        printf("%s\n", it->c_str());
}
