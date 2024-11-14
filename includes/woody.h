/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:06:57 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/14 11:46:16 by nguiard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOODY_H
#define	WOODY_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <stdbool.h>
#include <limits.h>

//	Utils
void	ft_memcpy(char *dst, char *src, size_t len);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
size_t	ft_strlen(const char *s);

//	Inject
int		inject_and_modify_entry(const char *input_file,
								const char *output_file,
								const unsigned int key);

//	Key generation
bool    generate_key(unsigned int *key, const char *key_string);

#endif
