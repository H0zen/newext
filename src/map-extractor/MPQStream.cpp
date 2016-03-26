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

#include "MPQStream.h"

MPQStream::MPQStream(ArchiveSet& _as) : m_eof(false), m_buff(NULL), m_size(0), m_pos(0), m_as(&_as)
{}

size_t MPQStream::read(void* dest, size_t bytes, size_t num)
{
    if (m_eof) return 0;

    size_t rpos;
    size_t i;

    for (i = 0; (i < num) && !m_eof; i++)
    { 
        rpos = m_pos + bytes;
        if (rpos > m_size)
        {
            bytes = m_size - m_pos;
            m_eof = true;
        }

        memcpy(dest, m_buff + m_pos, bytes);
        m_pos = rpos;
    }
    return i;
}

void MPQStream::seek(size_t offset, StreamPos pos)
{
    if (pos == POS_ABS)
        m_pos = offset;
    else
        m_pos += offset;

    m_eof = (m_pos >= m_size);
}

bool MPQStream::_open(char const* fileName)
{
    m_buff = m_as->ReadFile(fileName, m_size);
    return (m_buff != NULL);
}
