/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key_generation.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/14 10:58:42 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/14 11:43:46 by nguiard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

//	Generates a key either from user input or randomized
//
//	Returns true on error
bool generate_key(unsigned int *key, const char *key_string) {
	ssize_t			result;
	unsigned long	holder = 0;
	size_t			string_len = 0;
	int				urandom;

	if (key_string != NULL) {
		string_len = ft_strlen(key_string);
		if (string_len == 0) {
			fprintf(stderr, "The key string cannot be empty.\n");
		} else {
			if (string_len >= 2 && ft_strncmp(key_string, "0x", 2) == 0) {
				if (sscanf(key_string + 2, "%lx", &holder) != 1) {
					fprintf(stderr, "Could not parse hex key '\033[1m%s\033[0m'\n", key_string);
					goto generate;
				}
			} else {
				if (sscanf(key_string, "%lu", &holder) != 1) {
					fprintf(stderr, "Could not parse unsigned key '\033[1m%s\033[0m'\n", key_string);
					goto generate;
				}
			}
			if (holder > UINT_MAX) {
				fprintf(stderr, "The key would overflow (%lx > %x).\n", holder, UINT_MAX);
			} else {
				*key = (unsigned int)holder;
				printf("Using user key: 0x%08x.\n", *key);
				return false;
			}
		}
	}

generate:
	urandom = open("/dev/urandom", O_RDONLY);
	if (urandom < 0) {
		perror("open");
		return true;
	}

	result = read(urandom, key, sizeof(unsigned int));
	if (result < 0) {
		perror("read");
		close(urandom);
		return true;
	}
	
	printf("Using random key: 0x%08x.\n", *key);

	close(urandom);
	return false;
}
