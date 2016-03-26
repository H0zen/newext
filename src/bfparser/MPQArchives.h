#ifndef __I_STORAGE_H__
#define __I_STORAGE_H__

#include <string>
#include <vector>
#include <deque>

#include "common.h"
#include "StormLib.h"

class mpqArchives
{
    typedef std::vector<std::string> FileList;
    typedef std::deque<HANDLE> ArchiveSet;

  public:
    ~MPQArchives();

    bool Load(const FileList& mpqList);
    bool Patch(const FileList& patchList);
    void Free();

    bool ReadFile(const std::string& fileName, uint8*& buffer, size_t& size);
    bool ExtractFile(const std::string& inFileName, const std::string& outFileName);

    template <class Worker> void DoForAllFileType(const std::string extension, Worker& worker);

  private:
    void PrintLastErrMsg(const std::string& when);
    ArchiveSet m_store;
};
#endif
