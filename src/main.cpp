#include "Utils.h"
#include "DBCFile.h"

#include <cstdio>

int main(int argc, char** argv)
{
  uint32 build;
  int8 core;
  char const path[] = "./";
  FileListFeeder f;
  ArchiveSet archive;

  if (!GetCoreAndBuildNumber(build,core))
  {
      std::printf("Could not detect build and core");
      return 0;
  }

  if (!f.Init(path, build, core))
  {
      std::printf("Could not get filelist for processing");
      return 0;
  }

  if (!archive.Init(f))
  {
      std::printf("Could not load filelists for processing");
      return 0;
  }

  std::printf("\n\n\n Testing Map.dbc\n");
  std::printf("\n=========================\n");

  DBCFile dbc(archive);

  if (!dbc.open("DBFilesClient\\Map.dbc"))
  {
      std::printf("Could not read Map.dbc");
      return 0;
  }

  uint32 nrec = dbc.getRecordCount();
  std::printf("\n=========================\n");
  std::printf("\n\n Records in Map.dbc:\n");
  std::printf("\n=========================\n");

  for (uint32 i = 0; i < nrec; i++)
  {
      DBCFile::Record r = dbc.getRecord(i);
      std::printf("%u\t%s\n", r.getUInt32(0), r.getString(1));
  }

  std::printf("Success\n");

}
