/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/11/26 11:37:49 by amerelo           #+#    #+#             */
/*   Updated: 2017/08/15 02:46:12 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	int	test;

	test = n;
	while (n--)
	{
		*((char *)(dst)) = *((const char *)(src));
		dst++;
		src++;
	}
	dst -= n;
	src -= n;
	return (dst);
}
