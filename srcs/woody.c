#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <string.h>
#include <time.h>
#include <sys/syscall.h>  // Pour utiliser les syscalls

// Fonction pour vérifier que le fichier est un ELF 64 bits

// Fonction pour générer une clé de chiffrement aléatoire
void generate_key(unsigned char *key, size_t key_size) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        perror("Failed to open /dev/urandom");
        exit(EXIT_FAILURE);
    }
    if (read(fd, key, key_size) != key_size) {
        perror("Failed to read random data for key");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);

    printf("Generated encryption key: ");
    for (size_t i = 0; i < key_size; i++) {
        printf("%02x", key[i]);
    }
    printf("\n");
}


// Stream cipher encryption using XOR with a generated keystream
void stream_encrypt(void *data, size_t size, unsigned char *key, size_t key_size) {
    unsigned char *ptr = (unsigned char *)data;
    for (size_t i = 0; i < size; i++) {
        ptr[i] ^= key[i % key_size];
    }
}

// Function to encrypt the ELF file and save the encrypted file
int encrypt_elf(const char *input_filename, const char *output_filename) {
    int fd_in = open(input_filename, O_RDONLY);
    if (fd_in == -1) {
        perror("Failed to open input file");
        return -1;
    }

    off_t file_size = lseek(fd_in, 0, SEEK_END);
    if (file_size == -1) {
        perror("Failed to get file size");
        close(fd_in);
        return -1;
    }
    lseek(fd_in, 0, SEEK_SET);

    void *file_data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd_in, 0);
    if (file_data == MAP_FAILED) {
        perror("Failed to mmap file");
        close(fd_in);
        return -1;
    }

    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
    if (ehdr->e_ident[0] != 0x7f || ehdr->e_ident[1] != 'E' || ehdr->e_ident[2] != 'L' || ehdr->e_ident[3] != 'F') {
        fprintf(stderr, "Not a valid ELF file: Incorrect magic number\n");
        munmap(file_data, file_size);
        close(fd_in);
        return -1;
    }

    size_t key_size = 16;
    unsigned char key[key_size];
    generate_key(key, key_size);

    Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)file_data + ehdr->e_shoff);
    char *strtab = (char *)file_data + shdr[ehdr->e_shstrndx].sh_offset;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (shdr[i].sh_type == SHT_PROGBITS) {
            const char *section_name = strtab + shdr[i].sh_name;
            if (strcmp(section_name, ".text") == 0 || strcmp(section_name, ".data") == 0) {
                stream_encrypt((char *)file_data + shdr[i].sh_offset, shdr[i].sh_size, key, key_size);
                printf("Encrypted section: %s\n", section_name);
            }
        }
    }

    int fd_out = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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

    if (syscall(SYS_chmod, output_filename, 0755) == -1) {
        perror("Failed to set file permissions");
        munmap(file_data, file_size);
        close(fd_in);
        close(fd_out);
        return -1;
    }

    munmap(file_data, file_size);
    close(fd_in);
    close(fd_out);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Vérification du fichier ELF
    if (check_elf(argv[1]) == 0) {
        printf("The file is a valid ELF64 file.\n");
    } else {
        // fprintf(stderr, "The file is not a valid ELF64 file.\n");
        return EXIT_FAILURE;
    }

    // Nom du fichier de sortie (chiffré)
    const char *output_filename = "woody_crypted";

    // Appeler la fonction de chiffrement
    if (encrypt_elf(argv[1], output_filename) == 0) {
        printf("File has been successfully encrypted and saved as '%s'.\n", output_filename);
    } else {
        fprintf(stderr, "Encryption failed.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
