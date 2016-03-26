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

#include "DBCFile.h"
#undef min
#undef max

DBCFile::DBCFile(ArchiveSet& _as): MPQStream(_as), recordSize(0), recordCount(0), fieldCount(0), stringSize(0), stringTable(NULL), data(NULL)
{}

DBCFile::~DBCFile()
{
  if (data)
      delete [] data;
}

bool DBCFile::open(const std::string& dbcFile)
{
    if (!_open(dbcFile.c_str()))
        return false;

    do
    {
        uint32 header;
        if (read(&header, 1, 4) != 4) //header
        {
            printf("Could not read header in DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (header != 'WDBC')
        {
            printf("The header in DBCFile %s did not match.\n", dbcFile.c_str());
            continue;
        }

        if (read(&recordCount, 4, 1) != 1) // Number of records
        {
            printf("Could not read number of records from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (read(&fieldCount, 4, 1) != 1)  // Number of fields
        {
            printf("Could not read number of fields from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (read(&recordSize, 4, 1) != 1)  // Size of a record
        {
            printf("Could not read record size from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (read(&stringSize, 4, 1) != 1)  // String size
        {
            printf("Could not read string block size from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if ((fieldCount * 4) != recordSize)
        {
            printf("Field count and record size in DBCFile %s do not match.\n", dbcFile.c_str());
            continue;
        }

        size_t data_size = recordSize * recordCount + stringSize;
        data = new uint8[data_size];
        stringTable = data + recordSize * recordCount;

        if (read(data, 1, data_size) != data_size)
        {
            delete [] data;
            data = NULL;
            printf("DBCFile %s did not contain expected amount of data for records.\n", dbcFile.c_str());
        }
    }
    while(0);

    if (data)
        return true;

    return false;
}

DBCFile::Record DBCFile::getRecord(uint32 id)
{
    assert(data);
    return Record(*this, data + id * recordSize);
}

uint32 DBCFile::getMaxId()
{
    assert(data);

    uint32 maxId = 0;
    for (uint32 i = 0; i < getRecordCount(); ++i)
    {
        if (maxId < getRecord(i).getUInt32(0))
            { maxId = getRecord(i).getUInt32(0); }
    }
    return maxId;
}
