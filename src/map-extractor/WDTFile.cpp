
#include "WDTFile.h"

struct WDTAreaInfo
{
  uint32 flags;
  uint32 data;
};

WDTFile::WDTFile(ArchiveSet& _as)
{
    m_stream = new MPQStream(_as);
}

void WDTFile::reset()
{
  std::printf("\nCalling reset() on WDT\n");
  for (int i = 0; i < 64; i++)
    for (int j = 0; j < 64; j++)
        m_maps[i][j] = false;

  m_hasADT = false;

  filePrefix.clear();
}

bool WDTFile::open(char const* fileName)
{
  reset();
  
  if (!m_stream)
      return false;
  
  if (!m_stream->open(fileName))
      return false;

  if (!m_chunkReader.Load(m_stream))
      return false;
      
  std::string str(fileName);
  std::size_t pos = str.find(".wdt");
  filePrefix = str.substr(0, pos);

  WDTAreaInfo const* wai = (WDTAreaInfo const*)m_chunkReader.GetChunk('MAIN');
  if (wai)
  {
    for (int j = 0; j < 64; j++)
      for (int i = 0; i < 64; i++)
      {
        if (wai[i + j * 64].flags != 0)
          { m_maps[i][j] = true; m_hasADT = true; }
      }
  }
  return true;
}

void WDTFile::GetADTFileList()
{
  for (int i = 0; i < 64; i++)
    for (int j = 0; j < 64; j++)
      {
        if (m_maps[i][j])
          printf("%s_%d_%d.adt\n", filePrefix.c_str(), i, j);
      }
}

void WDTFile::GetADTFileList(std::vector<std::string>& list)
{
    char file[1024];

    if (m_hasADT)
    {
        for (int i = 0; i < 64; i++)
          for (int j = 0; j < 64; j++)
          {
              if (m_maps[i][j])
                {
                    std::sprintf(file, "%s_%d_%d.adt", filePrefix.c_str(), i, j);
                    list.push_back(file);
                }
          }
    }
    else
    {
        std::sprintf(file, "%s_%d_%d.adt", filePrefix.c_str(), 64, 64);
        list.push_back(file);
    }
}

