/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 15:05:53 by tischmid          #+#    #+#             */
/*   Updated: 2023/12/06 05:40:22 by tosuman          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putstr(char *s)
{
	int	i;

	if (!s)
		return (ft_putstr(NULL_PTR_STR));
	i = 0;
	while (*s && ft_putchar(*s++))
		++i;
	return (i);
}
