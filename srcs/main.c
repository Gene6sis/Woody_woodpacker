#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <string.h>

// Assembly code to print "Hello from Woody" and exit
unsigned char injected_code[] = {
    0xb8, 0x01, 0x00, 0x00, 0x00, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x48, 0x8d,
    0x35, 0x0c, 0x00, 0x00, 0x00, 0xba, 0x1a, 0x00, 0x00, 0x00, 0x0f, 0x05,
    0xe9, 0xe3, 0xff, 0xff, 0xff, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x66,
    0x72, 0x6f, 0x6d, 0x20, 0x69, 0x6e, 0x6a, 0x65, 0x63, 0x74, 0x65, 0x64,
    0x20, 0x63, 0x6f, 0x64, 0x65, 0x21, 0x0a
};
unsigned int injected_code_len = 55;



// Function to check ELF and find a cave in the .text section
Elf64_Shdr* find_text_section_cave(void *file_data, size_t *cave_offset) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
    Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)file_data + ehdr->e_shoff);
    char *strtab = (char *)file_data + shdr[ehdr->e_shstrndx].sh_offset;

    // Locate the .text section and find the cave
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (strcmp(strtab + shdr[i].sh_name, ".text") == 0) {
            // Check if the cave is big enough for the injected code
            if (shdr[i].sh_size >= injected_code_len) {
                *cave_offset = shdr[i].sh_offset + shdr[i].sh_size - injected_code_len;
                return &shdr[i];
            } else {
                fprintf(stderr, "Cave in .text section is not big enough for the injected code\n");
                return NULL;
            }
        }
    }
    return NULL;
}

// Function to inject code and modify entry point
int inject_and_modify_entry(const char *input_file, const char *output_file) {
    // Open original ELF file
    int fd_in = open(input_file, O_RDONLY);
    if (fd_in == -1) {
        perror("Failed to open input file");
        return -1;
    }

    // Map the file into memory
    off_t file_size = lseek(fd_in, 0, SEEK_END);
    void *file_data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd_in, 0);
    if (file_data == MAP_FAILED) {
        perror("Failed to mmap file");
        close(fd_in);
        return -1;
    }

    // Get original entry point
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
    Elf64_Addr original_entry = ehdr->e_entry;

    // Locate the .text section and find cave offset
    size_t cave_offset;
    Elf64_Shdr *text_section = find_text_section_cave(file_data, &cave_offset);
    if (!text_section) {
        fprintf(stderr, ".text section or cave not found\n");
        munmap(file_data, file_size);
        close(fd_in);
        return -1;
    }

    // Inject code into the cave
    memcpy((char *)file_data + cave_offset, injected_code, injected_code_len);

    // Print original entry point
    printf("Original entry point: 0x%lx\n", (unsigned long)original_entry);

    // Print cave offset and address
    printf("Cave offset: 0x%lx\n", (unsigned long)cave_offset);
    printf("Cave address: 0x%lx\n", (unsigned long)(text_section->sh_addr + (cave_offset - text_section->sh_offset)));

    // Print injected code details
    printf("Injected code length: %u bytes\n", injected_code_len);
    printf("Injected code starts at offset: 0x%lx\n", (unsigned long)cave_offset);

    // printf("Old entry point address placeholder: 0x%lx\n", *(Elf64_Addr *)(file_data + cave_offset + 62));

    // Replace placeholder with the actual original entry address
    // *(Elf64_Addr *)(file_data + cave_offset + 62) = original_entry; // Offset 62 is where placeholder starts


    // printf("Original entry point address injected into the code: 0x%lx\n", *(Elf64_Addr *)(file_data + cave_offset + 62));

    // Update the ELF header with the new entry point
    ehdr->e_entry = text_section->sh_addr + (cave_offset - text_section->sh_offset);

    // printf("New entry point set to: 0x%lx\n", (unsigned long)ehdr->e_entry);

    // Write modified ELF data to a new file
    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("Failed to open output file");
        munmap(file_data, file_size);
        close(fd_in);
        return -1;
    }

    if (write(fd_out, file_data, file_size) != file_size) {
        perror("Failed to write to output file");
        munmap(file_data, file_size);
        close(fd_in);
        close(fd_out);
        return -1;
    }

    printf("New ELF file created with modified entry point at offset: 0x%lx\n", (unsigned long)ehdr->e_entry);

    // Cleanup
    munmap(file_data, file_size);
    close(fd_in);
    close(fd_out);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input ELF file> <output ELF file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (inject_and_modify_entry(argv[1], argv[2]) == 0) {
        printf("File successfully modified and saved as '%s'\n", argv[2]);
    } else {
        fprintf(stderr, "Failed to modify the ELF file\n");
    }

    return EXIT_SUCCESS;
}
