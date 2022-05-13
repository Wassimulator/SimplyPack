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

bool Running = true;
int main(int argc, char **argv)
{
    bool scanned = false;
    bool detailed = false;

    char packed_dir[256];
    tinydir_dir dir_info;
    uint64_t filecount = 0;
    if (argc < 3 || argc > 4)
    {
        printf("Usage: [source directory] [destination file path] [detailed (0 or 1)]. argc = %i\n", argc);
        Running = false;
    }
    if (argc == 4)
    {
        if (atoi(argv[3]) == 1)
        {
            detailed = true;
        }
    }

    while (Running)
    {
        // scanning ///////////////////
        if (!scanned)
        {
            if (tinydir_open(&dir_info, (const TCHAR *)argv[1]) == -1)
            {
                printf("Directory not found!\n");
                Running = false;
            }
            while (dir_info.has_next)
            {
                tinydir_file file;
                tinydir_readfile(&dir_info, &file);
                if (!file.is_dir)
                {
                    filecount++;
                }
                tinydir_next(&dir_info);
            }
            scanned = true;
            tinydir_close(&dir_info);
        }

        // packing ///////////////////
        if (scanned)
        {
            FILE *Target = fopen(argv[2], "wb");
            if (!Target)
            {
                printf("Failed to open target file!\n");
                Running = false;
            }
            fwrite(&filecount, sizeof(uint64_t), 1, Target);

            tinydir_open(&dir_info, (const TCHAR *)argv[1]);
            fseek(Target, sizeof(SP_fileinfo) * filecount, SEEK_CUR);

            long at_header = sizeof(uint64_t); // skip first element for count
            while (dir_info.has_next)
            {
                long at = ftell(Target);
                tinydir_file file;
                tinydir_readfile(&dir_info, &file);
                if (!file.is_dir)
                {
                    char filepath[256];
                    sprintf(filepath, "%s/%s", argv[1], file.name);

                    // open file and get size info
                    FILE *f = fopen(filepath, "rb");
                    fseek(f, 0, SEEK_END);
                    uint64_t filesize = ftell(f);
                    fseek(f, 0, SEEK_SET);

                    // record file info in a temporary struct
                    SP_fileinfo info;
                    sprintf(info.filename, "%s", file.name);
                    info.filesize = filesize;
                    info.location = at - 8;

                    // write file info to target file header
                    fseek(Target, at_header, SEEK_SET);
                    at_header += sizeof(SP_fileinfo);

                    fwrite(&info, sizeof(SP_fileinfo), 1, Target);

                    // write file to target file
                    char *buffer = (char *)malloc(filesize);
                    fread(buffer, filesize, 1, f);
                    fseek(Target, at, SEEK_SET);
                    fwrite(buffer, filesize, 1, Target);

                    free(buffer);
                    fclose(f);
                    if (detailed)
                        printf("Packing: %s\n", file.name);
                }
                tinydir_next(&dir_info);
            }
            if (detailed)
                printf("\n");
            fclose(Target);
            printf("Packing complete, total files: %I64i in %s\n", filecount, argv[2]);
            if (detailed)
                printf("\n");
            Running = false;
        }
    }

    return 0;
}