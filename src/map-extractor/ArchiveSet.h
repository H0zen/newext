//MaNGOS header

#ifndef __ARCHIVE_SET_H__
#define __ARCHIVE_SET_H__

#include "TypeDefines.h"
#include "FileListFeeder.h"
#include "StormLib.h"
#include <vector>
#include <deque>

class ArchiveSet
{
    public:
        explicit ArchiveSet(FileListFeeder& _feeder);
        ~ArchiveSet();
        bool   Init();
//        uint8* ReadFile(const char* fileName, size_t& size);
//        bool   ExtractFile(const char* archiveFile, const char* physicalFile);
    private:
        std::deque<HANDLE> m_mpqList;
        FileListFeeder& m_feeder;
};
#endif