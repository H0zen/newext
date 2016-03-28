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

#include "DBFile.h"

DBCFile::DBCFile(ArchiveSet& _as): m_stream(NULL), recordSize(0), recordCount(0), fieldCount(0), stringSize(0), stringTable(NULL), recordData(NULL)
{
    m_stream = new MPQStream(_as);
    m_name.clear();
}

DBCFile::~DBCFile()
{
  if (m_stream)
      delete m_stream;
  m_stream = NULL;
}

void DBCFile::reset()
{
    // reset state for reuse case
    recordSize = 0;
    recordCount = 0;
    fieldCount = 0;
    stringSize = 0;
    stringTable = NULL;
    recordData = NULL;
    m_name.clear();
}

bool DBCFile::open(const std::string& dbcFile)
{
    reset();
    
    if (!m_stream)
        return false;

    if (!m_stream->open(dbcFile.c_str()))
        return false;

    do
    {
        uint32 header = 0;
        if (m_stream->read(&header, 4, 1) != 1) //header
        {
            printf("Could not read header in DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (header != 'CBDW')
        {
            printf("The header in DBCFile %s did not match.\n", dbcFile.c_str());
            continue;
        }

        if (m_stream->read(&recordCount, 4, 1) != 1) // Number of records
        {
            printf("Could not read number of records from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (m_stream->read(&fieldCount, 4, 1) != 1)  // Number of fields
        {
            printf("Could not read number of fields from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (m_stream->read(&recordSize, 4, 1) != 1)  // Size of a record
        {
            printf("Could not read record size from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if (m_stream->read(&stringSize, 4, 1) != 1)  // String size
        {
            printf("Could not read string block size from DBCFile %s.\n", dbcFile.c_str());
            continue;
        }

        if ((recordSize*recordCount + stringSize + 20) >=  m_stream->getSize())
        {
            printf("Read error in DBCFile %s.\n", dbcFile.c_str());
            continue;
        }
        recordData = m_stream->getBuffer(true);
        stringTable = recordData + recordSize * recordCount;
    }
    while(0);

    if (recordData)
    {
        std::size_t pos = dbcFile.find_last_of("\\");

        m_name = dbcFile.substr(pos + 1);

        return true;
    }
    return false;
}

bool DBCFile::write(const std::string& path)
{
    std::FILE* file = NULL;
    std::string fullName = path + "/"+ m_name;
    bool ok = true;

    file = std::fopen((fullName).c_str(), "wb");
    if (!file)
        return false;

    if (std::fwrite(m_stream->getBuffer(false), 1, m_stream->getSize(), file) != m_stream->getSize())
        ok = false;
    std::fclose(file);
    return ok;
}

DBCFile::Record DBCFile::getRecord(uint32 id)
{
    return Record(*this, recordData + id * recordSize);
}

uint32 DBCFile::getMaxId()
{
    uint32 maxId = 0;
    for (uint32 i = 0; i < getRecordCount(); ++i)
    {
        if (maxId < getRecord(i).getUInt32(0))
            { maxId = getRecord(i).getUInt32(0); }
    }
    return maxId;
}
uint32 DBCFile::getMinId()
{
    return 0; // not right
}

DB2File::DB2File(ArchiveSet& _as) : DBCFile(_as), table_hash(0), build(0), timestamp(0), minId(0), maxId(0), locale(0), copyTableSize(0)
{
}

DB2File::~DB2File()
{
}

void DB2File::reset()
{
    DBCFile::reset();
    table_hash = 0;
    build = 0;
    timestamp = 0;
    minId = 0;
    maxId = 0;
    locale = 0;
    copyTableSize = 0;
}

bool DB2File::open(const std::string& db2File)
{
    reset();

    if (!m_stream)
        return false;

    if (!m_stream->open(db2File.c_str()))
        return false;

    do
    {
        uint32 header = 0;
        if (m_stream->read(&header, 4, 1) != 1) //header
        {
            printf("Could not read header in DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (header != '2BDW')
        {
            printf("The header in DB2File %s did not match.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&recordCount, 4, 1) != 1) // Number of records
        {
            printf("Could not read number of records from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&fieldCount, 4, 1) != 1)  // Number of fields
        {
            printf("Could not read number of fields from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&recordSize, 4, 1) != 1)  // Size of a record
        {
            printf("Could not read record size from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&stringSize, 4, 1) != 1)  // String size
        {
            printf("Could not read string block size from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&table_hash, 4, 1) != 1)  // Table hash
        {
            printf("Could not read tableHash from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&build, 4, 1) != 1)  // Build version
        {
            printf("Could not read build version from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&timestamp, 4, 1) != 1)  // TimeStamp
        {
            printf("Could not read timestamp from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&minId, 4, 1) != 1)  // minId
        {
            printf("Could not read minId from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&maxId, 4, 1) != 1)  // maxId
        {
            printf("Could not read maxId from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&locale, 4, 1) != 1)  // Locale
        {
            printf("Could not read locale from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if (m_stream->read(&copyTableSize, 4, 1) != 1)  // CopyTableSize
        {
            printf("Could not read copy_table_size from DB2File %s.\n", db2File.c_str());
            continue;
        }

        if ((recordSize*recordCount + stringSize + 48) >= m_stream->getSize())
        {
            printf("Read error in DB2File %s. RecSize is %lu, RecCount is %lu, StringSize is %lu, GetSize() is: %lu\n", db2File.c_str(), recordSize, recordCount, stringSize, m_stream->getSize());
            continue;
        }

        recordData = m_stream->getBuffer(true);
        stringTable = recordData + recordSize * recordCount;
    }
    while(0);

    if (recordData)
    {
        std::size_t pos = db2File.find_last_of("\\");

        m_name = db2File.substr(pos + 1);

        return true;
    }
    return false;
}
