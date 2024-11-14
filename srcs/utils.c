/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:10:24 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/14 11:35:17 by nguiard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

void	ft_memcpy(char *dst, char *src, size_t len) {
	for (size_t i = 0; i < len; i++)
		dst[i] = src[i];
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (s1[i] && s2[i] && i < n)
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	if (s1[i] != s2[i] && i < n)
		return ((unsigned char)s1[i] - (unsigned char)s2[i]);
	return (0);
}

size_t	ft_strlen(const char *s) {
	size_t i = 0;

	for (; s[i]; i++) {;}

	return i;
}
