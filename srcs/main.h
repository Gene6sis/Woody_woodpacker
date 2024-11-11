#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <stdbool.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define Verbose 1
#define PAYLOAD_SIZE 50

typedef enum s_file_type
{
    NONE,
    FT_32,
    FT_64
}	t_file_type;

typedef struct s_elf_file
{
    Elf64_Ehdr	*header;
    Elf64_Phdr	*programs;
    Elf64_Shdr	*sections;
    Elf64_Shdr	*shstrtab;
} t_elf_file;

typedef struct s_file
{
	t_file_type	type;
	int			fd;
	void		*map;
	off_t		size;
	t_elf_file		*elf_file;
}	t_file;

typedef struct s_injection
{
	int 	fd; // file descriptor of the infected file it's a copy of the original one
	char	*file_map; // map of the infected file
	off_t	file_size; // size of the infected file
	off_t	encrypt_offset; // offset where the encryption will start
	int		encrypt_size; // size of the encryption
	size_t	old_entrypoint; // original entrypoint of the infected file
	size_t 	new_entrypoint; // new entrypoint of the infected file
	off_t	payload_offset; // offset where the payload will be injected
}   t_injection;
