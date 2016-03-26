#ifndef __FILELIST_FEEDER_H__
#define __FILELIST_FEEDER_H__

#include "TypeDefines.h"
#include "Utils.h"

#include <vector>
#include <string>

#define LOCALE_COUNT 12

enum FileListType
{
  FLT_BASE          = 0,
  FLT_LOCALE        = 1,
  FLT_BASE_PATCH    = 2,
  FLT_LOCALE_PATCH  = 3,
  FLT_NUM           = 4,
};

typedef std::vector<std::string> FileList;

class FileListFeeder
{
    public:
        explicit FileListFeeder(char const *path, uint32 build, int8 core);
        ~FileListFeeder();
        bool Init();
        FileList const& GetFileList(FileListType idx);
        std::string const& GetLocaleString() const { return m_listLocale[m_curLocale]; }
    private:
        void scan_patches(std::string& prefix, FileListType idx);
        void list_files(FileListType idx);
    private:
        std::vector<std::string> m_listLocale;
        std::vector<FileList>    m_list;
        int8                     m_curLocale;
        int8                     m_core;
        uint32                   m_build;
        char const               *m_path;
};

#endif