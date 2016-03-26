#include "MPQArchives.h"
#include <cstdio>

MPQArchives::~MPQArchives() 
{
    std::printf("In Archive destructor\n");
    Free();
}

void MPQArchives::PrintLastErrMsg(const std::string& when)
{
      std::printf("[ERROR]@%s: error code: %d\n", when.c_str(), GetLastError());
}

bool MPQArchives::Load(const FileList& mpqList)
{
    HANDLE mpqHandle;
    for (FileList::const_iterator it = mpqList.begin(); it != mpqList.end(); ++it)
    {
        if (!SFileOpenArchive((*it).c_str(), 0, MPQ_OPEN_READ_ONLY, &mpqHandle))
        {
            PrintLastErrMsg(std::string("MPQArchives::Load: ") + *it);
            return false;
        }

        m_store.push_back(mpqHandle);
    }
    return true;
}

bool MPQArchives::Patch(const FileList& patchList)
{
      //TODO for M3+
}

void MPQArchives::Free()
{
    std::printf("In MPQArchives::Free\n");

    for (ArchiveSet::const_iterator it = m_store.begin(); it != m_store.end(); ++it)
      { SFileCloseArchive(*it); }
    m_store.clear();
}


bool MPQArchives::ReadFile(const std::string& fileName, uint8*& buff, size_t& size)
{
    HANDLE fh;
    for (ArchiveSet::const_reverse_iterator it = m_store.rbegin(); it != m_store.rend(); ++it)
    {
        if (SFileOpenFileEx(*it, fileName.c_str(), SFILE_OPEN_FROM_MPQ, &fh))
        {
            // check size
            DWORD hiSize;
            if (((size = SFileGetFileSize(fh, &hiSize)) == SFILE_INVALID_SIZE) || (!size))
            {
              SFileCloseFile(fh);
              continue;
            }
                
            // allocate buffer for reading
            buff = new uint8[size];
                
            // try read into buffer
            DWORD readSize;
            if (!SFileReadFile(fh, buff, size, &readSize, NULL))
            {
                delete[] buff;
                SFileCloseFile(fh);
                continue;
            }
                
            // all ok, close file handle
            SFileCloseFile(fh);
            return true;
        }
    }

    // no file found into the MPQArchives
    printf("Error: '%s' not found into the MPQArchives!\n", fileName.c_str());
    return false;
}

bool MPQArchives::ExtractFile(const std::string& inFileName, const std::string& outFileName)
{
    for (ArchiveSet::const_reverse_iterator it = m_store.rbegin(); it != m_store.rend(); ++it)
      if (SFileExtractFile(*it, inFileName.c_str(), outFileName.c_str(), 0))
        { return true; }

    // no file found into the MPQArchives
    printf("Error: '%s' could not be extracted!\n", inFileName.c_str());
    return false;
}


template <class Worker> 
void MPQArchives::DoForAllFileType(const std::string extension, Worker& worker)
{}


