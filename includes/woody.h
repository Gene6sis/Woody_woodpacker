/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:06:57 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/13 14:28:18 by nguiard          ###   ########.fr       */
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

#define key_size 32

//	Utils
void	ft_memcpy(char *dst, char *src, size_t len);
int		ft_strncmp(const char *s1, const char *s2, size_t n);

//	Inject
int		inject_and_modify_entry(const char *input_file, const char *output_file);

//	Key generation
bool    generate_key(unsigned char *key, size_t size);

#endif
