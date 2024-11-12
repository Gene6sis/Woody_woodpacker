/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:21:46 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/12 14:47:08 by nguiard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input ELF file> <output ELF file>\n", argv[0]);
        return EXIT_FAILURE;
    }



    // unsigned char key[key_size];
    // generate_key(key, key_size);

    if (inject_and_modify_entry(argv[1], argv[2]) == 0) {
        printf("File successfully modified and saved as '%s'\n", argv[2]);
    } else {
        fprintf(stderr, "Failed to modify the ELF file\n");
    }

    return EXIT_SUCCESS;
}
