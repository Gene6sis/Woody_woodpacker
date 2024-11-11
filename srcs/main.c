#include "main.h"

int open_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return fd;
}

void free_file(t_file *file) {
    if (file->elf_file) {
        if (file->elf_file->header) {
            free(file->elf_file->header);
            file->elf_file->header = NULL;
        }
        if (file->elf_file->programs) {
            free(file->elf_file->programs);
            file->elf_file->programs = NULL;
        }
        if (file->elf_file->sections) {
            free(file->elf_file->sections);
            file->elf_file->sections = NULL;
        }
        free(file->elf_file);
        file->elf_file = NULL;
    }
    close(file->fd);
}

void free_injection(t_injection *injection) {
    if (injection->fd != -1) {
        close(injection->fd);
    }
}

t_file_type check_elf(int fd, t_file *file) {
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

bool parse_header(t_file *file, t_elf_file *elf_file)
{
    Elf64_Ehdr *header = malloc(sizeof(Elf64_Ehdr));
    if (!header)
    {
        perror("Error allocating memory");
        return false;
    }
    if (lseek(file->fd, 0, SEEK_SET) == -1)
    {
        free(header);
        perror("Error seeking file");
        return false;
    }
    if (read(file->fd, header, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr))
    {
        free(header);
        perror("Error reading file");
        return false;
    }
    elf_file->header = header;

    if (Verbose)
    {
        printf("ELF Header:\n");
        printf("  Magic:   ");
        for (int i = 0; i < EI_NIDENT; i++)
            printf("%02x ", elf_file->header->e_ident[i]);
        printf("\n");
        printf("  Class:                             %s\n", elf_file->header->e_ident[EI_CLASS] == ELFCLASS32 ? "ELF32" : "ELF64");
        printf("  Data:                              %s\n", elf_file->header->e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian");
        printf("  Version:                           %d\n", elf_file->header->e_ident[EI_VERSION]);
        printf("  OS/ABI:                            %d\n", elf_file->header->e_ident[EI_OSABI]);
        printf("  ABI Version:                       %d\n", elf_file->header->e_ident[EI_ABIVERSION]);
        printf("  Type:                              %d\n", elf_file->header->e_type);
        printf("  Machine:                           %d\n", elf_file->header->e_machine);
        printf("  Version:                           0x%x\n", elf_file->header->e_version);
        printf("  Entry point address:               0x%lx\n", elf_file->header->e_entry);
        printf("  Start of program headers:          %ld (bytes into file)\n", elf_file->header->e_phoff);
        printf("  Start of section headers:          %ld (bytes into file)\n", elf_file->header->e_shoff);
        printf("  Flags:                             0x%x\n", elf_file->header->e_flags);
        printf("  Size of this header:               %d (bytes)\n", elf_file->header->e_ehsize);
        printf("  Size of program headers:           %d (bytes)\n", elf_file->header->e_phentsize);
        printf("  Number of program headers:         %d\n", elf_file->header->e_phnum);
        printf("  Size of section headers:           %d (bytes)\n", elf_file->header->e_shentsize);
        printf("  Number of section headers:         %d\n", elf_file->header->e_shnum);
        printf("  Section header string table index: %d\n", elf_file->header->e_shstrndx);
    }
    return true;
}

bool parse_programs(t_file *file, t_elf_file *elf_file, Elf64_Ehdr *header)
{
    Elf64_Phdr *programs = malloc(sizeof(Elf64_Phdr) * header->e_phnum);
    if (!programs)
    {
        perror("Error allocating memory");
        return false;
    }
    if (lseek(file->fd, header->e_phoff, SEEK_SET) == -1)
    {
        free(programs);
        perror("Error seeking file");
        return false;
    }
    if (read(file->fd, programs, sizeof(Elf64_Phdr) * header->e_phnum) != sizeof(Elf64_Phdr) * header->e_phnum)
    {
        free(programs);
        perror("Error reading file");
        return false;
    }
    elf_file->programs = programs;

    if (Verbose)
    {
        printf("Program Headers:\n");
        for (int i = 0; i < header->e_phnum; i++)
        {
            printf("  Entry %d\n", i);
            printf("  Type:   %i\n", programs[i].p_type);
            // printf("  Offset: 0x%lx\n", programs[i].p_offset);
            // printf("  VirtAddr: 0x%lx\n", programs[i].p_vaddr);
            // printf("  PhysAddr: 0x%lx\n", programs[i].p_paddr);
            // printf("  FileSiz: %ld\n", programs[i].p_filesz);
            // printf("  MemSiz:  %ld\n", programs[i].p_memsz);
            // printf("  Flags:   %d\n", programs[i].p_flags);
            // printf("  Align:   %ld\n\n", programs[i].p_align);
        }
    }
    return true;
}


bool parse_sections(t_file *file, t_elf_file *elf_file, Elf64_Ehdr *header)
{
    Elf64_Shdr *sections = malloc(sizeof(Elf64_Shdr) * header->e_shnum);
    if (!sections)
    {
        perror("Error allocating memory");
        return false;
    }
    if (lseek(file->fd, header->e_shoff, SEEK_SET) == -1)
    {
        free(sections);
        perror("Error seeking file");
        return false;
    }
    if (read(file->fd, sections, sizeof(Elf64_Shdr) * header->e_shnum) != sizeof(Elf64_Shdr) * header->e_shnum)
    {
        free(sections);
        perror("Error reading file");
        return false;
    }
    elf_file->sections = sections;
    elf_file->shstrtab = &sections[header->e_shstrndx];
    return true;
}


bool file_information(t_file *file)
{
    t_elf_file *elf_file = malloc(sizeof(t_elf_file));
    if (!elf_file)
    {
        perror("Error allocating memory");
        return false;
    }
    elf_file->header = NULL;
	elf_file->programs = NULL;
	elf_file->sections = NULL;
    file->elf_file = elf_file;

    lseek(file->fd, 0, SEEK_SET);
    if (!parse_header(file, elf_file))
    {
        free_file(file);
        return false;
    }

    if (!parse_programs(file, elf_file, elf_file->header))
    {
        free(elf_file->header);
        free(elf_file);
        return false;
    }

    if (!parse_sections(file, elf_file, elf_file->header))
    {
        free(elf_file->header);
        free(elf_file->programs);
        free(elf_file);
        return false;
    }

    lseek(file->fd, 0, SEEK_SET);
	file->size = lseek(file->fd, 0, SEEK_END);
    if (file->size == -1) {
        perror("Cannot seek to end of file");
		return false;
    }

    file->map = mmap(NULL, file->size, PROT_READ, MAP_PRIVATE, file->fd, 0);
    if (file->map == MAP_FAILED) {
        perror("Cannot map file");
        return false;
    }

    return true;
}

Elf64_Shdr* find_text_section_cave(t_file *file, size_t *cave_offset) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
    Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)file_data + file->elf_file->header->e_shoff);
    char *strtab = (char *)file_data + shdr[ehdr->e_shstrndx].sh_offset;

    // Locate the .text section and find the cave
    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (strcmp(strtab + shdr[i].sh_name, ".text") == 0) {
            // Check if the cave is big enough for the injected code
            if (shdr[i].sh_size >= 50) {
                *cave_offset = shdr[i].sh_offset + shdr[i].sh_size - 50;
                return &shdr[i];
            } else {
                fprintf(stderr, "Cave in .text section is not big enough for the injected code\n");
                return NULL;
            }
        }
    }
    return NULL;
}


bool prepare_injection(t_injection *injection, t_file *file)
{
    injection->old_entrypoint = file->elf_file->header->e_entry;

    // Find a cave in sections
    Elf64_Phdr *code_cave = NULL;
    for (int i = 0; i < file->elf_file->header->e_phnum - 1; i++) {
		if (file->elf_file->programs[i].p_type != PT_LOAD)
			continue;
		if (!(file->elf_file->programs[i].p_flags & PF_X))
			continue;
		if (file->elf_file->programs[i + 1].p_type != PT_LOAD)
			continue;
		size_t available_space = file->elf_file->programs[i + 1].p_offset - (file->elf_file->programs[i].p_offset + file->elf_file->programs[i].p_memsz);
		if (available_space < PAYLOAD_SIZE)
			continue;
		code_cave = &(file->elf_file->programs[i]);
        break;
	}
    if (!code_cave) {
        fprintf(stderr, "No code cave found\n");
        return false;
    }

    // injection->encrypt_offset = code_cave->p_offset + code_cave->p_memsz;
    // injection->encrypt_size = PAYLOAD_SIZE;
    injection->payload_offset = code_cave->p_offset + code_cave->p_memsz;
    injection->new_entrypoint = code_cave->p_vaddr + code_cave->p_memsz;

    // Create a copy of the original file
    char *filename = "woody";
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening file");
        return false;
    }
    if (ftruncate(fd, file->size) == -1) {
        perror("Error truncating file");
        close(fd);
        return false;
    }
    void *map = mmap(NULL, file->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return false;
    }
    memcpy(map, file->map, file->size);
    injection->fd = fd;
    injection->file_map = map;
    injection->file_size = file->size;

    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "Usage: %s <file1>\n", argv[0]);
        return EXIT_FAILURE;
    }

    t_file file;
    file.elf_file = NULL;
    // Open file
    file.fd = open_file(argv[1]);
    if (file.fd == -1) {
        return EXIT_FAILURE;
    }

    // Check if the file is a valid ELF file and return the type of ELF file or -1
    file.type = check_elf(file.fd, &file);
    if (file.type == NONE) {
        free_file(&file);
        return EXIT_FAILURE;
    }
    
    if (Verbose)
    {
        if (file.type == FT_32)
            write(1, "ELF 32-bit file\n", 16);
        else if (file.type == FT_64)
            write(1, "ELF 64-bit file\n", 16);
    }

    if (!file_information(&file)) {
        free_file(&file);
        return EXIT_FAILURE;
    }

    if (Verbose)
        printf("File information obtained\n");

    t_injection injection;
    injection.fd = -1;
    if (!prepare_injection(&injection, &file)) {
        free_file(&file);
        return EXIT_FAILURE;
    }

    free_file(&file);
    free_injection(&injection);
    return 0;
}