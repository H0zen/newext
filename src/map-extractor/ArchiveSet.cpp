
#include "ArchiveSet.h"

ArchiveSet::ArchiveSet(FileListFeeder& _feeder) : m_feeder(_feeder), m_mpqList()
{
}

ArchiveSet::~ArchiveSet()
{
    for (std::deque<HANDLE>::const_iterator it = m_mpqList.begin(); it != m_mpqList.end(); ++it)
        SFileCloseArchive(*it);

    m_mpqList.clear();
}

bool ArchiveSet::Init()
{
    FileList f_list;
    FileList f_patch;

    HANDLE handle = NULL;

    std::printf("\n\n Loading data files\n");
    std::printf("==================\n");

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
    std::printf("==================\n");

    return true;
}
