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

#include "ArchiveSet.h"

ArchiveSet::ArchiveSet() : m_mpqList()
{
}

ArchiveSet::~ArchiveSet()
{
    for (std::deque<HANDLE>::const_iterator it = m_mpqList.begin(); it != m_mpqList.end(); ++it)
        SFileCloseArchive(*it);

    m_mpqList.clear();
}

bool ArchiveSet::Init(FileListFeeder& m_feeder)
{
    FileList f_list;
    FileList f_patch;

    HANDLE handle = NULL;

    std::printf("\n\n Loading data files\n");
    std::printf(" ==================\n");

    //open base files and patch them
    f_list = m_feeder.GetFileList(FLT_BASE);
    f_patch = m_feeder.GetFileList(FLT_BASE_PATCH);

    for (FileList::const_iterator it = f_list.begin(); it != f_list.end(); ++it)
    {
        if (!SFileOpenArchive(it->c_str(), 0, MPQ_OPEN_READ_ONLY, &handle))
        {
            std::printf(" Cannot open %s. Exiting\n", it->c_str());
            return false;
        }
        for (FileList::const_iterator ip = f_patch.begin(); ip != f_patch.end(); ++ip)
        {
            std::printf(" Apply patch %s to %s...", ip->c_str(), it->c_str());
            if (!SFileOpenPatchArchive(handle, ip->c_str(), "base", 0))
            {
                std::printf("failed! Will exit.\n");
                return false;
            }
            std::printf("OK\n");
        }
        m_mpqList.push_back(handle);
    }

    //open locale files and patch them
    f_list = m_feeder.GetFileList(FLT_LOCALE);
    f_patch = m_feeder.GetFileList(FLT_LOCALE_PATCH);

    for (FileList::const_iterator it = f_list.begin(); it != f_list.end(); ++it)
    {
        if (!SFileOpenArchive(it->c_str(), 0, MPQ_OPEN_READ_ONLY, &handle))
        {
            std::printf(" Cannot open %s. Exiting\n", it->c_str());
            return false;
        }

        for (FileList::const_iterator ip = f_patch.begin(); ip != f_patch.end(); ++ip)
        {
            std::printf(" Apply patch %s to %s: ", ip->c_str(), it->c_str());
            if (!SFileOpenPatchArchive(handle, ip->c_str(), NULL, 0))
            {
                std::printf("failed! Will exit.\n");
                return false;
            }
            std::printf("OK\n");
        }
        m_mpqList.push_back(handle);
    }
    std::printf("\n\n Done!\n");
    std::printf(" ==================\n");

    return true;
}

uint8* ArchiveSet::ReadFile(char const* fileName, size_t& size)
{
    HANDLE fh;
    uint8* buff = NULL;

    for (std::deque<HANDLE>::const_reverse_iterator it = m_mpqList.rbegin(); it != m_mpqList.rend(); ++it)
    {
        if (!SFileOpenFileEx(*it, fileName, SFILE_OPEN_PATCHED_FILE, &fh))
            continue;
        do
        {
            // check size
            DWORD hiSize;
            if (((size = SFileGetFileSize(fh, &hiSize)) == SFILE_INVALID_SIZE) || (!size))
                continue;

            // allocate buffer for reading
            buff = new uint8[size];

            // try read into buffer
            DWORD readSize;
            if (!SFileReadFile(fh, buff, size, &readSize, NULL))
            {
                delete [] buff;
                buff = NULL;
            }
        }
        while(0);

        //close file handle
        SFileCloseFile(fh);

        if (buff) { break; }
    }

    // no file found
    if (!buff)
        printf(" Error: '%s' not found\n", fileName);

    return buff;
}

