/*
   _____ _                 __      ____             __  
  / ___/(_)___ ___  ____  / /_  __/ __ \____ ______/ /__
  \__ \/ / __ `__ \/ __ \/ / / / / /_/ / __ `/ ___/ //_/
 ___/ / / / / / / / /_/ / / /_/ / ____/ /_/ / /__/ ,<   
/____/_/_/ /_/ /_/ .___/_/\__, /_/    \__,_/\___/_/|_|  
                /_/      /____/                         

        v1.0
        by: Wassimulator

    A very simple and straightforward package manager for games written in C.
    Use SimplyPack.exe or compile simplypack.cpp to use the packer, this header only includes the unpacking algorithms.

    SimplyPack takes 3 arguments:
        - The path to the folder from which to pack files
        - The path to the output file
        - 1 or 0 to display detailed output of what was packed (optional)

    The packer is pretty simple, and will pack everything inside the folder you specify. reserving a header at the beginning that
    contains the following information consecutively:

    - The number of files packed 
    - An array of structs of the type 'SP_fileinfo' which contain:
        - original file name
        - original file size
        - packed file location (relative to the beginning of the pack file)

    To use the unpacker make sure you first call `SP_OpenPack` with the path to the pack file. This call copies the pack into memory.
    'SP_GetData' gets you a pointer to the data stored in memory, you should use this pointer to read the data, make sure you copy it.
    `SP_ClosePack` frees the memory used by the pack.

    The algorithm doesn't hash but instead matches file names. This tool intended for smaller projects.

    Dependencies: tinydir.h, included in the project, please read the copyright information for that in the file.
    Licensing info at the bottom.
*/


#pragma once
#include <tinydir.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>

struct SP_fileinfo
{
    char filename[64];
    uint64_t filesize;
    uint64_t location;
};

struct SP_packerdata
{
    FILE *file;
    uint64_t packsize = 0;
    bool pack_opened = false;
    uint64_t filecount = 0;
    uint8_t *buffer;
};

SP_packerdata SP_PACKERDATA;

int SP_OpenPack(char *file)
{
    SP_packerdata *P = &SP_PACKERDATA;
    P->file = fopen(file, "rb");
    if (!P->file)
    {
        printf("Failed to open pack file: %s\n", file);
        return -1;
    }
    fseek(P->file, 0, SEEK_END);
    P->packsize = ftell(P->file);
    fseek(P->file, 0, SEEK_SET);

    P->pack_opened = true;
    fread(&P->filecount, sizeof(uint64_t), 1, P->file);
    P->buffer = (uint8_t *)malloc(P->packsize);
    fread(P->buffer, P->packsize, 1, P->file);

    fclose(P->file);
    return 0;
}
void SP_ClosePack()
{
    SP_packerdata *P = &SP_PACKERDATA;
    P->pack_opened = false;
    free(P->buffer);
}

uint8_t *SP_GetData(char *filename, int *size)
{
    SP_packerdata *P = &SP_PACKERDATA;
    if (!P->pack_opened)
    {
        printf("Pack file not opened!\n");
        return NULL;
    }
    int index = -1;
    for (uint64_t i = 0; i < P->filecount; i++)
    {
        SP_fileinfo *info = (SP_fileinfo *)&P->buffer[i * sizeof(SP_fileinfo)];
        if (strcmp(info->filename, filename) == 0)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        printf("File not found in pack file!\n");
        return NULL;
    }
    SP_fileinfo *info = (SP_fileinfo *)&P->buffer[index * sizeof(SP_fileinfo)];
    *size = info->filesize;
    return &(uint8_t)P->buffer[info->location];
}


/*
    LICENSE: SimplyPack

    Copyright (c) 2022 Wassim Alhajomar / Mahmoud Wasim Alhaj Omar (@Wassimulator)

    This software is provided "as-is", without any express or implied warranty. In no event
    will the authors be held liable for any damages arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose, including commercial
    applications, and to alter it and redistribute it freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not claim that you
        wrote the original software. If you use this software in a product, an acknowledgment
        in the product documentation would be appreciated but is not required.

        2. Altered source versions must be plainly marked as such, and must not be misrepresented
        as being the original software.

        3. This notice may not be removed or altered from any source distribution.

        4. Distribution in binary form must include the above copyright statement.
*/