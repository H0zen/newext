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

#ifndef __DB_FILE_H__
#define __DB_FILE_H__

#include "MPQStream.h"

#include <cstdio>
#include <string>

class DBCFile
{
    public:
        explicit DBCFile(ArchiveSet&);
        virtual ~DBCFile();

        virtual bool open(const std::string& dbcFile);
        bool write(const std::string& path);

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
                    return *(float*)(offset + (field * 4));
                }

                uint32 getUInt32(uint32 field) const
                {
                    return *(uint32*)(offset + (field * 4));
                }

                int32 getInt32(uint32 field) const
                {
                    return *(int32*)(offset + (field * 4));
                }

                uint8 getByte(uint32 ofs) const
                {
                    return *(offset + ofs);
                }

                const char* getString(uint32 field) const
                {
                    uint32 stringOffset = getUInt32(field);
                    return (const char*)(file.stringTable + stringOffset);
                }
            private:
                Record(DBCFile& file, uint8 const* offset): file(file), offset(offset) {}
                DBCFile& file;
                uint8 const* offset;

                friend class DBCFile;
        };

        Record getRecord(uint32 id);
        uint32 getRecordCount() const { return recordCount;}
        uint32 getFieldCount() const { return fieldCount; }
        virtual uint32 getMaxId();
        virtual uint32 getMinId();
    protected:
        virtual void reset();
    protected:
        MPQStream*   m_stream;
        uint8 const  *stringTable;
        uint8 const  *recordData;
        std::string  m_name;
        uint32       recordSize;
        uint32       recordCount;
        uint32       fieldCount;
        uint32       stringSize;
};


class DB2File : public DBCFile
{
      public:
          explicit DB2File(ArchiveSet& _as);
          virtual ~DB2File();
          virtual bool open(const std::string& db2File);
          virtual uint32 getMaxId() { return maxId; }
          virtual uint32 getMinId() { return minId; }
      private:
          virtual void reset();
      private:
          uint32    table_hash;
          uint32    build;
          uint32    timestamp;
          uint32    minId;
          uint32    maxId;
          uint32    locale;
          uint32    copyTableSize;
};

#endif
