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

#ifndef __STREAM__H__
#define __STREAM__H__

#include "ArchiveSet.h"

enum StreamPos
{
    POS_ABS = 0, //absolute
    POS_REL = 1, //relative
};

class MPQStream
{
    public:
        explicit MPQStream(ArchiveSet& _as);
        virtual ~MPQStream() { if (m_buff) delete [] m_buff; }

        void    seek(size_t offset, StreamPos type);
        size_t  read(void* dest, size_t size, size_t num);

        bool open(char const* fileName);
        bool eof() const { return m_eof; }
        uint8 const* getBuffer(bool at) const;
        size_t getSize() const { return m_size; }
    private:
        ArchiveSet  *m_as;
        uint8*      m_buff;
        size_t      m_pos;
        size_t      m_size;
        bool        m_eof;
};

#endif
