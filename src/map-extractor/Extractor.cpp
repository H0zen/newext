
#include "Extractor.h"
#include "DBCFile.h"


Extractor::Extractor() : m_as(NULL), m_core(-1), m_build(0), m_type(0)
{
    inPath = std::string(".");
    outPath = std::string(".");
}


Extractor::~Extractor()
{
  if (m_as)
      delete m_as;
  m_as = NULL;
}

void Extractor::Usage(char* prg)
{
    std::printf(" Usage: %s [OPTION]\n\n", prg);
    std::printf(" Extract client database files and generate map files.\n");
    std::printf("   -h, --help            show the usage\n");
    std::printf("   -i, --input <path>    search path for game client archives\n");
    std::printf("   -o, --output <path>   target path for generated files\n");
    std::printf("   -e, --extract #       extract specified client data. 0 = all, 1 = DBCs, 2 = MAPS,\n");
    std::printf("                         3 = VMAPS. Defaults to extracting all.\n");
    std::printf("\n");
    std::printf(" Example:\n");
    std::printf(" - use input path and extract only VMAPS:\n");
    std::printf("   %s -e 3 -i \"c:\\games\\world of warcraft\"\n", prg);
}

bool Extractor::Init(int argc, char** argv)
{
    char* param = NULL;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0)
        {
            param = argv[++i];
            if (!param)
            {
                Usage(argv[0]);
                return false;
            }

            inPath = std::string(param);
        }

        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
        {
            param = argv[++i];
            if (!param)
            {
                Usage(argv[0]);
                return false;
            }

            outPath = std::string(param);

        }

        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--extract") == 0)
        {
            param = argv[++i];
            if (!param)
            {
                Usage(argv[0]);
                return false;
            }

            int convertExtract = atoi(param);

            if (convertExtract > 0 && convertExtract < 4)
                m_type = convertExtract;
            else
            {
                Usage(argv[0]);
                return false;
            }
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            Usage(argv[0]);
            return false;
        }
    }


    if (!GetCoreAndBuildNumber(inPath, m_build, m_core))
    {
        return false;
    }

    ShowBanner("map-extractor", m_core);

    FileListFeeder feeder;

    if (!feeder.Init(inPath.c_str(), m_build, m_core))
        return false;

    m_as = new ArchiveSet();
    if (!m_as->Init(feeder))
    {
        printf("ArchiveSet init failed\n");
        delete m_as;
        return false;
    }

    return true;
}


void Extractor::Run()
{
    switch (m_type)
    {
        case 0: //ALL
        {
            ExtractDBC();
            //ExtractMAPS();
            //ExtractVMAPS();
            break;
        }
        case 1: //DBC
            ExtractDBC();
            break;
        case 2: //MAPS
            //ExtractMAPS();
            break;
        case 3:
            //ExtractVMAPS();
            break;
        default:
            break;
    }
}

void Extractor::ExtractDBC()
{
    std::string out = outPath + "/dbc";

    CreateDir(out);

    std::set<std::string> dbcList;

    if (!m_as->GetFileList("*.dbc", dbcList))
        std::printf("Cannot get dbc listfile\n");
    if (m_core == CLIENT_CATA)
        if (!m_as->GetFileList("*.db2", dbcList))
        std::printf("Cannot get db2 listfile\n");

    uint32 count = 0;

    DBCFile dbc(*m_as);
    for (std::set<std::string>::const_iterator it = dbcList.begin(); it != dbcList.end(); ++it)
    {
      if (dbc.open(it->c_str()))
      {
          std::printf("Extracting: %s\n", it->c_str());
          dbc.write(out.c_str());
          count++;
      }
    }
    std::printf("Extracted %u dbc files\n\n", count);
}
