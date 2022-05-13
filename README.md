# SimplyPack              

        v1.0
        by: Wassimulator

    A very simple and straightforward package manager for games written in C.
    Use SimplyPack.exe or compile simplypack.cpp to use the packer, this header only includes the unpacking algorithms.

    SimplyPack takes 3 arguments:
        - The path to the folder from which to pack files
        - The path to the output file
        - 1 or 0 to display detailed output of what was packed (optional)

## the Packer
    The packer is pretty simple, and will pack everything inside the folder you specify. reserving a header at the beginning that
    contains the following information consecutively:
    - The number of files packed 
    - An array of structs of the type 'SP_fileinfo' which contain:
        - original file name
        - original file size
        - packed file location (relative to the beginning of the pack file)

    The algorithm doesn't hash but instead matches file names. This tool intended for smaller projects.

    Dependencies: tinydir.h, included in the project, please read the copyright information for that in the file.
    Licensing info at the bottom.