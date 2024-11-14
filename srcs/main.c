/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:21:46 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/14 11:45:53 by nguiard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

int main(int argc, char *argv[]) {
	unsigned int	key;

	if (!(argc == 3 || argc == 4)) {
        fprintf(stderr, "Usage: %s <input ELF file> <output ELF file> [key]\n", argv[0]);
        return EXIT_FAILURE;
    }

	if (generate_key(&key, argv[3])) {
		fprintf(stderr, "Error generating a key\n");
		return EXIT_FAILURE;
	}

    if (inject_and_modify_entry(argv[1], argv[2], key) == 0) {
        printf("File successfully modified and saved as '%s'\n", argv[2]);
    } else {
        fprintf(stderr, "Failed to modify the ELF file\n");
    }

    return EXIT_SUCCESS;
}
