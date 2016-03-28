/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2016  MaNGOS project <http://getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

bool  CONF_allow_height_limit       = true;     /**< Allows to limit minimum height */
#define MIN_HEIGHT -500.0f

enum MapLiquidType
{
    MAP_LIQUID_TYPE_NO_WATER    = 0x00,
    MAP_LIQUID_TYPE_MAGMA       = 0x01,
    MAP_LIQUID_TYPE_OCEAN       = 0x02,
    MAP_LIQUID_TYPE_SLIME       = 0x04,
    MAP_LIQUID_TYPE_WATER       = 0x08,
    MAP_LIQUID_TYPE_DARK_WATER  = 0x10,
    MAP_LIQUID_TYPE_WMO_WATER   = 0x20,
};


// Map file format data
static char const MAP_MAGIC[]           = "MAPS"; /**< TODO */
static char       MAP_VERSION_MAGIC[32] = "0000"; /**< TODO */
static char const MAP_AREA_MAGIC[]      = "AREA"; /**< TODO */
static char const MAP_HEIGHT_MAGIC[]    = "MHGT"; /**< TODO */
static char const MAP_LIQUID_MAGIC[]    = "MLIQ"; /**< TODO */

/**
 * @brief
 *
 */
struct map_fileheader
{
    uint32 mapMagic;        /**< TODO */
    uint32 versionMagic;    /**< TODO */
    uint32 buildMagic;
    uint32 areaMapOffset;   /**< TODO */
    uint32 areaMapSize;     /**< TODO */
    uint32 heightMapOffset; /**< TODO */
    uint32 heightMapSize;   /**< TODO */
    uint32 liquidMapOffset; /**< TODO */
    uint32 liquidMapSize;   /**< TODO */
    uint32 holesOffset;     /**< TODO */
    uint32 holesSize;       /**< TODO */
};

#define MAP_AREA_NO_AREA      0x0001
#define MAP_HEIGHT_NO_HEIGHT  0x0001
#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

/**
 * @brief
 *
 */
struct map_areaHeader
{
    uint32 fourcc;          /**< TODO */
    uint16 flags;           /**< TODO */
    uint16 gridArea;        /**< TODO */
};


/**
 * @brief
 *
 */
struct map_heightHeader
{
    uint32 fourcc;          /**< TODO */
    uint32 flags;           /**< TODO */
    float  gridHeight;      /**< TODO */
    float  gridMaxHeight;   /**< TODO */
};


/**
 * @brief
 *
 */
struct map_liquidHeader
{
    uint32 fourcc;          /**< TODO */
    uint16 flags;           /**< TODO */
    uint16 liquidType;      /**< TODO */
    uint8  offsetX;         /**< TODO */
    uint8  offsetY;         /**< TODO */
    uint8  width;           /**< TODO */
    uint8  height;          /**< TODO */
    float  liquidLevel;     /**< TODO */
};

//
// Adt file convertor function and data
//

/**
 * @brief
 *
 * @param filename
 * @param filename2
 * @param build
 * @return bool
 */
bool Extractor::ConvertADT(char const* in, char const* out)
{
    uint16 area_flags[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];
    uint16 liquid_entry[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];

    float V8[ADT_GRID_SIZE][ADT_GRID_SIZE];        
    float V9[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];
    float liquid_height[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];

    uint8 liquid_flags[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];
    bool  liquid_show[ADT_GRID_SIZE][ADT_GRID_SIZE];

    ADTFile adt(*m_as);

    if (!adt.open(in))
        { return false; }

    MCIN* cells = adt.a_grid->getMCIN();
    if (!cells)
    {
        printf("Can not find cells in '%s'\n", filename);
        return false;
    }

    memset(liquid_show, 0, sizeof(liquid_show));
    memset(liquid_flags, 0, sizeof(liquid_flags));
    memset(liquid_entry, 0, sizeof(liquid_entry));

    // Prepare map header
    map_fileheader map;
    map.mapMagic = *(uint32 const*)MAP_MAGIC;
    map.versionMagic = *(uint32 const*)MAP_VERSION_MAGIC;
    map.buildMagic = build;

    // Get area flags data
    for (int i = 0; i < ADT_CELLS_PER_GRID; i++)
    {
        for (int j = 0; j < ADT_CELLS_PER_GRID; j++)
        {
            adt_MCNK* cell = cells->getMCNK(i, j);
            uint32 areaid = cell->areaid;
            if (areaid && areaid <= maxAreaId)
            {
                if (areas[areaid] != 0xffff)
                {
                    area_flags[i][j] = areas[areaid];
                    continue;
                }
                printf("File: %s\nCan not find area flag for area %u [%d, %d].\n", filename, areaid, cell->ix, cell->iy);
            }
            area_flags[i][j] = 0xffff;
        }
    }
    //============================================
    // Try pack area data
    //============================================
    bool fullAreaData = false;
    uint32 areaflag = area_flags[0][0];
    for (int y = 0; y < ADT_CELLS_PER_GRID; y++)
    {
        for (int x = 0; x < ADT_CELLS_PER_GRID; x++)
        {
            if (area_flags[y][x] != areaflag)
            {
                fullAreaData = true;
                break;
            }
        }
    }

    map.areaMapOffset = sizeof(map);
    map.areaMapSize   = sizeof(map_areaHeader);

    map_areaHeader areaHeader;
    areaHeader.fourcc = *(uint32 const*)MAP_AREA_MAGIC;
    areaHeader.flags = 0;
    if (fullAreaData)
    {
        areaHeader.gridArea = 0;
        map.areaMapSize += sizeof(area_flags);
    }
    else
    {
        areaHeader.flags |= MAP_AREA_NO_AREA;
        areaHeader.gridArea = (uint16)areaflag;
    }

    //
    // Get Height map from grid
    //
    for (int i = 0; i < ADT_CELLS_PER_GRID; i++)
    {
        for (int j = 0; j < ADT_CELLS_PER_GRID; j++)
        {
            adt_MCNK* cell = cells->getMCNK(i, j);
            if (!cell)
                { continue; }
            // Height values for triangles stored in order:
            // 1     2     3     4     5     6     7     8     9
            //    10    11    12    13    14    15    16    17
            // 18    19    20    21    22    23    24    25    26
            //    27    28    29    30    31    32    33    34
            // . . . . . . . .
            // For better get height values merge it to V9 and V8 map
            // V9 height map:
            // 1     2     3     4     5     6     7     8     9
            // 18    19    20    21    22    23    24    25    26
            // . . . . . . . .
            // V8 height map:
            //    10    11    12    13    14    15    16    17
            //    27    28    29    30    31    32    33    34
            // . . . . . . . .

            // Set map height as grid height
            for (int y = 0; y <= ADT_CELL_SIZE; y++)
            {
                int cy = i * ADT_CELL_SIZE + y;
                for (int x = 0; x <= ADT_CELL_SIZE; x++)
                {
                    int cx = j * ADT_CELL_SIZE + x;
                    V9[cy][cx] = cell->ypos;
                }
            }
            for (int y = 0; y < ADT_CELL_SIZE; y++)
            {
                int cy = i * ADT_CELL_SIZE + y;
                for (int x = 0; x < ADT_CELL_SIZE; x++)
                {
                    int cx = j * ADT_CELL_SIZE + x;
                    V8[cy][cx] = cell->ypos;
                }
            }
            // Get custom height
            adt_MCVT* v = cell->getMCVT();
            if (!v)
                { continue; }
            // get V9 height map
            for (int y = 0; y <= ADT_CELL_SIZE; y++)
            {
                int cy = i * ADT_CELL_SIZE + y;
                for (int x = 0; x <= ADT_CELL_SIZE; x++)
                {
                    int cx = j * ADT_CELL_SIZE + x;
                    V9[cy][cx] += v->height_map[y * (ADT_CELL_SIZE * 2 + 1) + x];
                }
            }
            // get V8 height map
            for (int y = 0; y < ADT_CELL_SIZE; y++)
            {
                int cy = i * ADT_CELL_SIZE + y;
                for (int x = 0; x < ADT_CELL_SIZE; x++)
                {
                    int cx = j * ADT_CELL_SIZE + x;
                    V8[cy][cx] += v->height_map[y * (ADT_CELL_SIZE * 2 + 1) + ADT_CELL_SIZE + 1 + x];
                }
            }
        }
    }
    //============================================
    // Try pack height data
    //============================================
    float maxHeight = -20000;
    float minHeight =  20000;
    for (int y = 0; y < ADT_GRID_SIZE; y++)
    {
        for (int x = 0; x < ADT_GRID_SIZE; x++)
        {
            float h = V8[y][x];
            if (maxHeight < h) { maxHeight = h; }
            if (minHeight > h) { minHeight = h; }
        }
    }
    for (int y = 0; y <= ADT_GRID_SIZE; y++)
    {
        for (int x = 0; x <= ADT_GRID_SIZE; x++)
        {
            float h = V9[y][x];
            if (maxHeight < h) { maxHeight = h; }
            if (minHeight > h) { minHeight = h; }
        }
    }

    // Check for allow limit minimum height (not store height in deep ochean - allow save some memory)
    if (CONF_allow_height_limit && minHeight < CONF_use_minHeight)
    {
        for (int y = 0; y < ADT_GRID_SIZE; y++)
            for (int x = 0; x < ADT_GRID_SIZE; x++)
                if (V8[y][x] < CONF_use_minHeight)
                    { V8[y][x] = CONF_use_minHeight; }
        for (int y = 0; y <= ADT_GRID_SIZE; y++)
            for (int x = 0; x <= ADT_GRID_SIZE; x++)
                if (V9[y][x] < CONF_use_minHeight)
                    { V9[y][x] = CONF_use_minHeight; }
        if (minHeight < CONF_use_minHeight)
            { minHeight = CONF_use_minHeight; }
        if (maxHeight < CONF_use_minHeight)
            { maxHeight = CONF_use_minHeight; }
    }

    map.heightMapOffset = map.areaMapOffset + map.areaMapSize;
    map.heightMapSize = sizeof(map_heightHeader);

    map_heightHeader heightHeader;
    heightHeader.fourcc = *(uint32 const*)MAP_HEIGHT_MAGIC;
    heightHeader.flags = 0;
    heightHeader.gridHeight    = minHeight;
    heightHeader.gridMaxHeight = maxHeight;

    if (maxHeight == minHeight)
        { heightHeader.flags |= MAP_HEIGHT_NO_HEIGHT; }

    // Not need store if flat surface
    if (CONF_allow_float_to_int && (maxHeight - minHeight) < CONF_flat_height_delta_limit)
        { heightHeader.flags |= MAP_HEIGHT_NO_HEIGHT; }

    // Try store as packed in uint16 or uint8 values
    if (!(heightHeader.flags & MAP_HEIGHT_NO_HEIGHT))
    {
        map.heightMapSize += sizeof(V9) + sizeof(V8); }
    }

    // Get from MCLQ chunk (old)
    for (int i = 0; i < ADT_CELLS_PER_GRID; i++)
    {
        for (int j = 0; j < ADT_CELLS_PER_GRID; j++)
        {
            adt_MCNK* cell = cells->getMCNK(i, j);
            if (!cell)
                { continue; }

            adt_MCLQ* liquid = cell->getMCLQ();
            int count = 0;
            if (!liquid || cell->sizeMCLQ <= 8)
                { continue; }

            for (int y = 0; y < ADT_CELL_SIZE; y++)
            {
                int cy = i * ADT_CELL_SIZE + y;
                for (int x = 0; x < ADT_CELL_SIZE; x++)
                {
                    int cx = j * ADT_CELL_SIZE + x;
                    if (liquid->flags[y][x] != 0x0F)
                    {
                        liquid_show[cy][cx] = true;
                        if (liquid->flags[y][x] & (1 << 7))
                            { liquid_flags[i][j] |= MAP_LIQUID_TYPE_DARK_WATER; }
                        ++count;
                    }
                }
            }

            uint32 c_flag = cell->flags;
            if (c_flag & (1 << 2))
            {
                liquid_entry[i][j] = 1;
                liquid_flags[i][j] |= MAP_LIQUID_TYPE_WATER;            // water
            }
            if (c_flag & (1 << 3))
            {
                liquid_entry[i][j] = 2;
                liquid_flags[i][j] |= MAP_LIQUID_TYPE_OCEAN;            // ocean
            }
            if (c_flag & (1 << 4))
            {
                liquid_entry[i][j] = 3;
                liquid_flags[i][j] |= MAP_LIQUID_TYPE_MAGMA;            // magma/slime
            }

            if (!count && liquid_flags[i][j])
                { fprintf(stderr, "Wrong liquid type detected in MCLQ chunk"); }

            for (int y = 0; y <= ADT_CELL_SIZE; y++)
            {
                int cy = i * ADT_CELL_SIZE + y;
                for (int x = 0; x <= ADT_CELL_SIZE; x++)
                {
                    int cx = j * ADT_CELL_SIZE + x;
                    liquid_height[cy][cx] = liquid->liquid[y][x].height;
                }
            }
        }
    }

    // Get liquid map for grid (in WOTLK used MH2O chunk)
    adt_MH2O* h2o = adt.a_grid->getMH2O();
    if (h2o)
    {
        for (int i = 0; i < ADT_CELLS_PER_GRID; i++)
        {
            for (int j = 0; j < ADT_CELLS_PER_GRID; j++)
            {
                adt_liquid_header* h = h2o->getLiquidData(i, j);
                if (!h)
                    { continue; }

                int count = 0;
                uint64 show = h2o->getLiquidShowMap(h);
                for (int y = 0; y < h->height; y++)
                {
                    int cy = i * ADT_CELL_SIZE + y + h->yOffset;
                    for (int x = 0; x < h->width; x++)
                    {
                        int cx = j * ADT_CELL_SIZE + x + h->xOffset;
                        if (show & 1)
                        {
                            liquid_show[cy][cx] = true;
                            ++count;
                        }
                        show >>= 1;
                    }
                }

                liquid_entry[i][j] = h->liquidType;
                switch (LiqType[h->liquidType])
                {
                    case LIQUID_TYPE_WATER: liquid_flags[i][j] |= MAP_LIQUID_TYPE_WATER; break;
                    case LIQUID_TYPE_OCEAN: liquid_flags[i][j] |= MAP_LIQUID_TYPE_OCEAN; break;
                    case LIQUID_TYPE_MAGMA: liquid_flags[i][j] |= MAP_LIQUID_TYPE_MAGMA; break;
                    case LIQUID_TYPE_SLIME: liquid_flags[i][j] |= MAP_LIQUID_TYPE_SLIME; break;
                    default:
                        printf("\nCan not find liquid type %u for map %s\nchunk %d,%d\n", h->liquidType, filename, i, j);
                        break;
                }
                // Dark water detect
                if (LiqType[h->liquidType] == LIQUID_TYPE_OCEAN)
                {
                    uint8* lm = h2o->getLiquidLightMap(h);
                    if (!lm)
                        { liquid_flags[i][j] |= MAP_LIQUID_TYPE_DARK_WATER; }
                }

                if (!count && liquid_flags[i][j])
                    { printf("Wrong liquid type detected in MH2O chunk"); }

                float* height = h2o->getLiquidHeightMap(h);
                int pos = 0;
                for (int y = 0; y <= h->height; y++)
                {
                    int cy = i * ADT_CELL_SIZE + y + h->yOffset;
                    for (int x = 0; x <= h->width; x++)
                    {
                        int cx = j * ADT_CELL_SIZE + x + h->xOffset;
                        if (height)
                            { liquid_height[cy][cx] = height[pos]; }
                        else
                            { liquid_height[cy][cx] = h->heightLevel1; }
                        pos++;
                    }
                }
            }
        }
    }
    //============================================
    // Pack liquid data
    //============================================
    uint8 type = liquid_flags[0][0];
    bool fullType = false;
    for (int y = 0; y < ADT_CELLS_PER_GRID; y++)
    {
        for (int x = 0; x < ADT_CELLS_PER_GRID; x++)
        {
            if (liquid_flags[y][x] != type)
            {
                fullType = true;
                y = ADT_CELLS_PER_GRID;
                break;
            }
        }
    }

    map_liquidHeader liquidHeader;

    // no water data (if all grid have 0 liquid type)
    if (type == 0 && !fullType)
    {
        // No liquid data
        map.liquidMapOffset = 0;
        map.liquidMapSize   = 0;
    }
    else
    {
        int minX = 255, minY = 255;
        int maxX = 0, maxY = 0;
        maxHeight = -20000;
        minHeight = 20000;
        for (int y = 0; y < ADT_GRID_SIZE; y++)
        {
            for (int x = 0; x < ADT_GRID_SIZE; x++)
            {
                if (liquid_show[y][x])
                {
                    if (minX > x) { minX = x; }
                    if (maxX < x) { maxX = x; }
                    if (minY > y) { minY = y; }
                    if (maxY < y) { maxY = y; }
                    float h = liquid_height[y][x];
                    if (maxHeight < h) { maxHeight = h; }
                    if (minHeight > h) { minHeight = h; }
                }
                else
                    { liquid_height[y][x] = CONF_use_minHeight; }
            }
        }
        map.liquidMapOffset = map.heightMapOffset + map.heightMapSize;
        map.liquidMapSize = sizeof(map_liquidHeader);
        liquidHeader.fourcc = *(uint32 const*)MAP_LIQUID_MAGIC;
        liquidHeader.flags = 0;
        liquidHeader.liquidType = 0;
        liquidHeader.offsetX = minX;
        liquidHeader.offsetY = minY;
        liquidHeader.width   = maxX - minX + 1 + 1;
        liquidHeader.height  = maxY - minY + 1 + 1;
        liquidHeader.liquidLevel = minHeight;

        if (maxHeight == minHeight)
            { liquidHeader.flags |= MAP_LIQUID_NO_HEIGHT; }

        if (!fullType)
            { liquidHeader.flags |= MAP_LIQUID_NO_TYPE; }

        if (liquidHeader.flags & MAP_LIQUID_NO_TYPE)
            { liquidHeader.liquidType = type; }
        else
            { map.liquidMapSize += sizeof(liquid_entry) + sizeof(liquid_flags); }

        if (!(liquidHeader.flags & MAP_LIQUID_NO_HEIGHT))
            { map.liquidMapSize += sizeof(float) * liquidHeader.width * liquidHeader.height; }
    }

    // map hole info
    uint16 holes[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];

    if (map.liquidMapOffset)
        { map.holesOffset = map.liquidMapOffset + map.liquidMapSize; }
    else
        { map.holesOffset = map.heightMapOffset + map.heightMapSize; }

    map.holesSize = sizeof(holes);
    memset(holes, 0, map.holesSize);

    for (int i = 0; i < ADT_CELLS_PER_GRID; ++i)
    {
        for (int j = 0; j < ADT_CELLS_PER_GRID; ++j)
        {
            adt_MCNK* cell = cells->getMCNK(i, j);
            if (!cell)
                { continue; }
            holes[i][j] = cell->holes;
        }
    }


    // Ok all data prepared - store it
    FILE* output = fopen(filename2, "wb");
    if (!output)
    {
        printf("Can not create the output file '%s'\n", filename2);
        return false;
    }
    fwrite(&map, sizeof(map), 1, output);
    // Store area data
    fwrite(&areaHeader, sizeof(areaHeader), 1, output);
    if (!(areaHeader.flags & MAP_AREA_NO_AREA))
        { fwrite(area_flags, sizeof(area_flags), 1, output); }

    // Store height data
    fwrite(&heightHeader, sizeof(heightHeader), 1, output);
    if (!(heightHeader.flags & MAP_HEIGHT_NO_HEIGHT))
    {
            fwrite(V9, sizeof(V9), 1, output);
            fwrite(V8, sizeof(V8), 1, output);
    }

    // Store liquid data if need
    if (map.liquidMapOffset)
    {
        fwrite(&liquidHeader, sizeof(liquidHeader), 1, output);
        if (!(liquidHeader.flags & MAP_LIQUID_NO_TYPE))
        {
            fwrite(liquid_entry, sizeof(liquid_entry), 1, output);
            fwrite(liquid_flags, sizeof(liquid_flags), 1, output);
        }
        if (!(liquidHeader.flags & MAP_LIQUID_NO_HEIGHT))
        {
            for (int y = 0; y < liquidHeader.height; y++)
                { fwrite(&liquid_height[y + liquidHeader.offsetY][liquidHeader.offsetX], sizeof(float), liquidHeader.width, output); }
        }
    }

    // store hole data
    fwrite(holes, map.holesSize, 1, output);

    fclose(output);

    return true;
}
