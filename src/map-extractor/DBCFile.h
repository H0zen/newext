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

#ifndef DBCFILE_H
#define DBCFILE_H

#include "MPQStream.h"

#include <cassert>
#include <cstdio>
#include <string>

class DBCFile : public MPQStream
{
    public:
        DBCFile(ArchiveSet&);
        ~DBCFile();

        bool open(const std::string& dbcFile);

        class Record
        {
            public:
                Record& operator= (const Record& r)
                {
                    file = r.file;
                    offset = r.offset;
                    return *this;
                }

                float getFloat(uint32 field) const
                {
                    assert(field < file.fieldCount);
                    return *reinterpret_cast<float*>(offset + (field * 4));
                }

                uint32 getUInt32(uint32 field) const
                {
                    assert(field < file.fieldCount);
                    return *reinterpret_cast<uint32*>(offset + (field * 4));
                }

                int32 getInt32(uint32 field) const
                {
                    assert(field < file.fieldCount);
                    return *reinterpret_cast<int32*>(offset + (field * 4));
                }

                uint8 getByte(uint32 ofs) const
                {
                    assert(ofs < file.recordSize);
                    return *(offset + ofs);
                }

                const char* getString(uint32 field) const
                {
                    assert(field < file.fieldCount);
                    uint32 stringOffset = static_cast<uint32>(getUInt32(field));
                    assert(stringOffset < file.stringSize);
                    return reinterpret_cast<const char*>(file.stringTable + stringOffset);
                }
            private:
                Record(DBCFile& file, uint8* offset): file(file), offset(offset) {}
                DBCFile& file; /**< TODO */
                uint8* offset; /**< TODO */

                friend class DBCFile;
        };

        Record getRecord(uint32 id);
        uint32 getRecordCount() const { return recordCount;}
        uint32 getFieldCount() const { return fieldCount; }
        uint32 getMaxId();
    private:
        uint32      recordSize;
        uint32      recordCount;
        uint32      fieldCount;
        uint32      stringSize;
        uint8*      stringTable;
        uint8*      data;
};

#endif
