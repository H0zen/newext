
#include "ChunkParser.h"

#include <cstdio>
#include <string>


bool ChunkParser::Load(MPQStream* stream)
{
    bool ok = true;
    uint32 magic = 0;
    uint32 size = 0;

    while (ok && !stream->eof())
    {
        if (stream->read(&magic, sizeof(uint32), 1) != 1)
        {
            ok = false;
            continue;
        }

        if (stream->read(&size, sizeof(uint32), 1) != 1)
        {
            ok = false;
            continue;
        }
        
        m_chunks.insert(std::pair<uint32, uint8 const*>(magic, stream->getBuffer(true)));
        stream->seek(size, POS_REL);
    }
    return ok;
}

void ChunkParser::printChunkTable()
{
    for (ChunkTable::const_iterator it = m_chunks.begin(); it != m_chunks.end(); ++it)
    {
        std::printf("Chunk ID: 0x%X\tAdress:%p\n", it->first, it->second);
    }
}

uint8 const* ChunkParser::GetChunk(uint32 id)
{
    ChunkTable::const_iterator it = m_chunks.find(id);
    if (it == m_chunks.end())
        return NULL;
    else
        return it->second;
}
