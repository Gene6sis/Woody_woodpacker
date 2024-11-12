/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inject.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:25:32 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/12 15:04:00 by nguiard          ###   ########.fr       */
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

// Function to check ELF and find a cave in the .text section
Elf64_Phdr* find_text_section_cave(void *file_data, size_t *cave_offset) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
	Elf64_Phdr *phdr = (Elf64_Phdr *)((char *)file_data + ehdr->e_phoff);
	Elf64_Phdr *curr = NULL;

	for (size_t i = 0; i < ehdr->e_phnum; i++) {
		curr = &phdr[i];
		if (curr->p_type == PT_LOAD && curr->p_flags == PF_X + PF_R) {
			printf("Found the segment:\nStart: %lx\nSize: %lx\nEnd: %lx\n",
					curr->p_offset, curr->p_filesz, curr->p_offset + curr->p_filesz + curr->p_align - (curr->p_filesz % curr->p_align)
				);
			*cave_offset = curr->p_offset + curr->p_filesz;
			if (*cave_offset + injected_code_len >=
				(curr->p_offset + curr->p_filesz + curr->p_align - (curr->p_filesz % curr->p_align))
			)
			{
				return NULL;
			}
			curr->p_flags = PF_X + PF_R + PF_W;
			return curr;
		}
	}
	return NULL;
}

void	change_asm_variables(void *file_data, size_t original_entry, size_t cave_offset, size_t key, size_t text_size) {
	const unsigned int	jump_to_decrypted	= original_entry - (cave_offset + injected_code_len);
	const unsigned int	jump_to_start_text	= original_entry - (cave_offset + START_OFF + 4);
	const unsigned int	jump_to_end_text	= original_entry + text_size - (cave_offset + END_OFF + 4);
	const unsigned char	jump_4bytes_opcode	= 0xe9;

	(void)key;
	if (text_size == 0)
		return;

	printf("Changing last jump instruction from rip + 0x%x\n",
		*(unsigned int *)(file_data + cave_offset + injected_code_len - 4));
	printf("To 0x%x\n\n", jump_to_decrypted);

	memcpy(file_data + cave_offset + injected_code_len - 4, &jump_to_decrypted, 4);
	memcpy(file_data + cave_offset + injected_code_len - 5, &jump_4bytes_opcode, 1);

	printf("Start offset: %08x\n", *(unsigned int *)(file_data + cave_offset + START_OFF));	
	printf("End offset:   %08x\n", *(unsigned int *)(file_data + cave_offset + END_OFF));	
	printf("Key offset:   %08x\n", *(unsigned int *)(file_data + cave_offset + KEY_OFF));	

	memcpy(file_data + cave_offset + START_OFF, &jump_to_start_text, 4);
	memcpy(file_data + cave_offset + END_OFF, &jump_to_end_text, 4);
	memcpy(file_data + cave_offset + KEY_OFF, &key, 4);

	printf("Start offset: %08x\n", *(unsigned int *)(file_data + cave_offset + START_OFF));	
	printf("End offset:   %08x\n", *(unsigned int *)(file_data + cave_offset + END_OFF));	
	printf("Key offset:   %08x\n", *(unsigned int *)(file_data + cave_offset + KEY_OFF));	
}

size_t	find_text_size(void *file_data) {
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
	Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)file_data + ehdr->e_shoff);
	char *strtab = (char *)file_data + shdr[ehdr->e_shstrndx].sh_offset;

	for (int i = 0; i < ehdr->e_shnum; i++) {
		if (strcmp(strtab + shdr[i].sh_name, ".text") == 0) {
			return shdr[i].sh_size;
		}
	}
	return 0;
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
	Elf64_Phdr *load_segment = find_text_section_cave(file_data, &cave_offset);
	if (!load_segment) {
		fprintf(stderr, "code semgent not found\n"); 
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

	// Print injected code details
	printf("Injected code length: %u bytes\n", injected_code_len);
	printf("Injected code starts at offset: 0x%lx\n", (unsigned long)cave_offset);


	// printf("Old entry point address placeholder: 0x%lx\n", *(Elf64_Addr *)(file_data + cave_offset + 62));

	// Replace placeholder with the actual original entry address
	// *(Elf64_Addr *)(file_data + cave_offset + 62) = original_entry; // Offset 62 is where placeholder starts

	change_asm_variables(file_data, original_entry, cave_offset, 0x42, find_text_size(file_data));

	// printf("Original entry point address injected into the code: 0x%lx\n", *(Elf64_Addr *)(file_data + cave_offset + 62));

	// Update the ELF header with the new entry point
	ehdr->e_entry = cave_offset;

	printf("New entry point set to: 0x%lx\n", (unsigned long)ehdr->e_entry);
	print_memory((char *)(ehdr->e_entry + file_data), injected_code_len);
	puts("\n");

	// Write modified ELF data to a new file
	int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0755);
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