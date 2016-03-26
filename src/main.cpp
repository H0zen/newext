#include "Utils.h"
#include "FileListFeeder.h"
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
          printf("Success\n");
      }
  }
}
