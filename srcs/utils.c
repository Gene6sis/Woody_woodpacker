/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nguiard <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 14:10:24 by nguiard           #+#    #+#             */
/*   Updated: 2024/11/12 14:10:52 by nguiard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "woody.h"

void	print_memory(char *data, size_t len) {
	unsigned char	a;
	for (size_t i = 0; i < len; i++) {
		if (i % 16 == 0) {
			printf("\n");
		}
		else if (i % 8 == 0) {
			printf(" ");
		}
		a = data[i];
		printf("%02x ", a); 
	}
}
