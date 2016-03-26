
#include "MPQStream.h"

MPQStream::MPQStream(ArchiveSet& _as) : m_eof(false), m_buff(NULL), m_size(0), m_pos(0), m_as(&_as)
{}

size_t MPQStream::read(void* dest, size_t bytes, size_t num)
{
    if (m_eof) return 0;

    size_t rpos;
    size_t i;

    for (i = 0; (i < num) && !m_eof; i++)
    { 
        rpos = m_pos + bytes;
        if (rpos > m_size)
        {
            bytes = m_size - m_pos;
            m_eof = true;
        }

        memcpy(dest, m_buff + m_pos, bytes);
        m_pos = rpos;
    }
    return i;
}

void MPQStream::seek(size_t offset, StreamPos pos)
{
    if (pos == POS_ABS)
        m_pos = offset;
    else
        m_pos += offset;

    m_eof = (m_pos >= m_size);
}

bool MPQStream::_open(char const* fileName)
{
    m_buff = m_as->ReadFile(fileName, m_size);
    return (m_buff != NULL);
}
