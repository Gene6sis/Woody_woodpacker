#include "main.h"

int open_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return fd;
}

t_file_type check_elf(int fd) {
    // Read the first 4 bytes of the file
    char magic[4];
    if (read(fd, magic, 4) != 4) {
        perror("Error reading file");
        return NONE;
    }

    // Check if the file is an ELF file
    if (magic[0] != 0x7f || magic[1] != 'E' || magic[2] != 'L' || magic[3] != 'F') {
        fprintf(stderr, "Not an ELF file\n");
        return NONE;
    }

    // Check if the file is a 64-bit ELF file
    if (lseek(fd, 4, SEEK_SET) == -1) {
        perror("Error seeking file");
        return NONE;
    }
    if (read(fd, magic, 1) != 1) {
        perror("Error reading file");
        return NONE;
    }
    if (magic[0] == 1) {
        return FT_32; // 32-bit ELF file
    } else if (magic[0] == 2) {
        return FT_64; // 64-bit ELF file
    }
    return NONE; // Not a valid ELF file
}

bool file_information(t_file *file)
{
    
    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "Usage: %s <file1>\n", argv[0]);
        return EXIT_FAILURE;
    }

    t_file file;
    // Open file
    file.fd = open_file(argv[1]);
    if (file.fd == -1) {
        return EXIT_FAILURE;
    }

    // Check if the file is a valid ELF file and return the type of ELF file or -1
    file.type = check_elf(file.fd);
    if (file.type == NONE) {
        close(file.fd);
        return EXIT_FAILURE;
    }
    
    if (Verbose)
    {
        if (file.type == FT_32)
            printf("ELF 32-bit file\n");
        else if (file.type == FT_64)
            printf("ELF 64-bit file\n");
    }

    if (!file_information(&file)) {
        close(file.fd);
        return EXIT_FAILURE;
    }

    // Close file
    close(file.fd);
    return 0;
}