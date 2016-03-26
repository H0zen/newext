#include "Utils.h"
#include "ArchiveSet.h"

#include <cstdio>

int main(int argc, char** argv)
{
  uint32 build;
  int8 core;
  char const path[] = "./";

  if (GetCoreAndBuildNumber(build,core))
  {
      FileListFeeder f(path, build, core);
      if (f.Init())
      {
          ArchiveSet archive(f);
              if (archive.Init())
                printf("Success\n");
      }
  }
}
