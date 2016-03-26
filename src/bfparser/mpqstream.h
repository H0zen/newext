#ifndef __STREAM__H__
#define __STREAM__H__

#include <cstdio>

class mpqStream
{
    public:
        enum StreamPos
        {
            POS_START = 0,
            POS_REL = 1,
        };
    public:
        virtual ~mpqStream(){};
        virtual void seek(size_t offset, StreamPos type) = 0;
        virtual size_t read(void* dest, size_t size) = 0;
};


class mpqFileStream : public mpqStream
{
    public:
        explicit mpqFileStream(const char* fileName);
        virtual ~mpqFileStream();
        virtual void seek(size_t offset, StreamPos type);
        virtual size_t read (void* dest, size_t size);
        bool IsLoaded() const { return m_handle != NULL; }
    private:
        std::FILE* m_handle;
};

class mpqMemoryStream : public mpqStream
{
    public:
        explicit mpqMemoryStream(void* src, size_t size);
        virtual ~mpqMemoryStream() { delete[] m_buff; };
        virtual void seek(size_t offset, StreamPos type);
        virtual size_t read(void* dest, size_t size, int num);
        bool IsLoaded() const { return m_buff != NULL; }
    private:
        char* m_buff;
        size_t m_pos;
        size_t m_size;
        bool m_eof;
};
#endif