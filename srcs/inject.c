/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inject.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:25:32 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/13 11:19:45 by nguiard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

// Assembly code that prints "...WOODY...", decrypts the code and jumps to the decrypted code
unsigned char injected_code[] = {
	0x50, 0x53, 0x51, 0x52, 0x57, 0x56, 0x68, 0x2e, 0x2e, 0x2e, 0x0a, 0x68,
	0x4f, 0x4f, 0x44, 0x59, 0x68, 0x2e, 0x2e, 0x2e, 0x57, 0xb8, 0x01, 0x00,
	0x00, 0x00, 0xbf, 0x02, 0x00, 0x00, 0x00, 0x48, 0x89, 0xe6, 0xba, 0x14,
	0x00, 0x00, 0x00, 0x0f, 0x05, 0x58, 0x58, 0x58, 0x48, 0x8d, 0x05, 0xcd,
	0xff, 0xff, 0xff, 0x48, 0x8d, 0x1d, 0x21, 0x00, 0x00, 0x00, 0x48, 0x31,
	0xc9, 0xba, 0x00, 0x00, 0x00, 0x00, 0x48, 0x39, 0xd8, 0x73, 0x0c, 0x8b,
	0x08, 0x31, 0xd1, 0x89, 0x08, 0x48, 0x83, 0xc0, 0x04, 0xeb, 0xef, 0x5e,
	0x5f, 0x5a, 0x59, 0x5b, 0x58, 0xeb, 0xa5, 0xff, 0xff, 0xff
};
unsigned int injected_code_len = 94;

#define KEY_OFF		0x3e
#define START_OFF	0x2f
#define END_OFF		0x36

#define PF_WOODY	0b1000

Elf64_Phdr* find_text_section_cave(void *file_data, size_t *cave_offset) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
	Elf64_Phdr *phdr = (Elf64_Phdr *)((char *)file_data + ehdr->e_phoff);
	Elf64_Phdr *curr = NULL;

	for (size_t i = 0; i < ehdr->e_phnum; i++) {
		curr = &phdr[i];
		if (curr->p_type == PT_LOAD && curr->p_flags & PF_X && curr->p_flags & PF_R) {
			if (curr->p_flags & PF_WOODY) {
				fprintf(stderr, "This file has already been injected by woody.\n");
				return NULL;
			}
			*cave_offset = curr->p_offset + curr->p_filesz;
			if (*cave_offset + injected_code_len >=
				(curr->p_offset + curr->p_filesz + curr->p_align - (curr->p_filesz % curr->p_align))
			)
			{
				fprintf(stderr, "This file does not have enough size in the code segment to be infected.\n");
				return NULL;
			}
			curr->p_flags = PF_X + PF_R + PF_W + PF_WOODY; // On le "signe" pour pas re-infecter
			return curr;
		}
	}
	return NULL;
}

void	change_asm_variables(void *file_data, size_t original_entry, size_t cave_offset, unsigned int key, size_t text_size) {
	const unsigned int	jump_to_decrypted	= original_entry - (cave_offset + injected_code_len);
	const unsigned int	jump_to_start_text	= original_entry - (cave_offset + START_OFF + 4);
	const unsigned int	ptr_to_end_text		= original_entry + text_size - (cave_offset + END_OFF + 4);
	const unsigned char	jump_4bytes_opcode	= 0xe9;

	if (text_size == 0)
		return;

	ft_memcpy(file_data + cave_offset + injected_code_len - 4, (char *)&jump_to_decrypted, 4);
	ft_memcpy(file_data + cave_offset + injected_code_len - 5, (char *)&jump_4bytes_opcode, 1);
	ft_memcpy(file_data + cave_offset + START_OFF, (char *)&jump_to_start_text, 4);
	ft_memcpy(file_data + cave_offset + END_OFF, (char *)&ptr_to_end_text, 4);
	ft_memcpy(file_data + cave_offset + KEY_OFF, (char *)&key, 4);
}

void	find_text_size(void *file_data, size_t *start, size_t *size) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
	Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)file_data + ehdr->e_shoff);
	char *strtab = (char *)file_data + shdr[ehdr->e_shstrndx].sh_offset;

	for (int i = 0; i < ehdr->e_shnum; i++) {
		if (ft_strncmp(strtab + shdr[i].sh_name, ".text", 6) == 0) {
			*start = shdr[i].sh_offset;
			*size =  shdr[i].sh_size; 
			return;
		}
	}
}

void	encrypt(unsigned char *begin, size_t size, unsigned int key) {
	unsigned int	encrypted;
	unsigned char	*end = begin + size;

	while (begin < end) {
		encrypted = *((unsigned int *)begin);
		encrypted = encrypted ^ key;
		*((unsigned int *)begin) = encrypted;
		begin += 4;
	}
}

bool	valid_file(const unsigned char *file, const size_t file_size) {
	if (file_size < sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) + 0x1000) {
		return false;
	}

	if (((uint32_t *)file)[0] != 0x464c457f)
		return false;

	if (file[EI_CLASS] != ELFCLASS64 || file[EI_DATA] != ELFDATA2LSB)
		return false;

	if (((Elf64_Ehdr *)file)->e_machine != EM_X86_64)
		return false;

	return true;
}

// Function to inject code and modify entry point
int inject_and_modify_entry(const char *input_file, const char *output_file) {
	int			fd_in;
	int			fd_out;
	void		*file_data;
	off_t		file_size;
	size_t		text_begin;
	size_t		text_size;
	size_t		cave_offset;
	Elf64_Ehdr	*ehdr;
	Elf64_Addr	original_entry;
	Elf64_Phdr	*load_segment;
	uint32_t	key = 0x1122aabb;

	fd_in = open(input_file, O_RDONLY);
	if (fd_in == -1) {
		perror("Failed to open input file");
		return -1;
	}

	file_size = lseek(fd_in, 0, SEEK_END);
	file_data = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd_in, 0);
	if (file_data == MAP_FAILED) {
		perror("Failed to mmap file");
		close(fd_in);
		return -1;
	}

	if (valid_file(file_data, file_size) == false) {
		fprintf(stderr, "This is not a falid ELF file\n");
		munmap(file_data, file_size);
		close(fd_in);
		return -1;
	}

	ehdr = (Elf64_Ehdr *)file_data;
	original_entry = ehdr->e_entry;

	load_segment = find_text_section_cave(file_data, &cave_offset);
	cave_offset += 4;
	if (!load_segment) {
		fprintf(stderr, "Error finding the code semgent.\n");
		munmap(file_data, file_size);
		close(fd_in);
		return -1;
	}

	find_text_size(file_data, &text_begin, &text_size);

	encrypt(file_data + original_entry, text_size - (original_entry - text_begin), key);
	
	ft_memcpy((char *)(file_data + cave_offset), (char *)injected_code, injected_code_len);
	
	change_asm_variables(file_data, original_entry, cave_offset, key, text_size - (original_entry - text_begin));
	ehdr->e_entry = cave_offset;

	fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0755);
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

	munmap(file_data, file_size);
	close(fd_in);
	close(fd_out);

	return 0;
}
