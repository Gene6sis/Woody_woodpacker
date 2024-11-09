#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <stdbool.h>

#define Verbose 1

typedef enum s_file_type
{
    NONE,
    FT_32,
    FT_64
}	t_file_type;

typedef struct s_elf_file
{
    Elf64_Ehdr	*header;
    Elf64_Shdr	*sections;
    Elf64_Phdr	*segments;
} t_elf_file;

typedef struct s_file
{
	t_file_type	type;
	int			fd;
	void		*map;
	off_t		size;
	t_elf_file		*elf_file;
}	t_file;

