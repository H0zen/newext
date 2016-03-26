#ifndef __STREAM__H__
#define __STREAM__H__

#include "ArchiveSet.h"

enum StreamPos
{
    POS_ABS = 0, //absolute
    POS_REL = 1, //relative
};

class MPQStream
{
    public:
        explicit MPQStream(ArchiveSet& _as);
        virtual ~MPQStream() { if (m_buff) delete [] m_buff; }

        void    seek(size_t offset, StreamPos type);
        size_t  read(void* dest, size_t size, size_t num);

    protected:
        bool _open(char const* fileName);

    private:
        ArchiveSet  *m_as;
        uint8*      m_buff;
        size_t      m_pos;
        size_t      m_size;
        bool        m_eof;
};

#endif