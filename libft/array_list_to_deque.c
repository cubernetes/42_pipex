/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   array_list_to_deque.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/19 14:25:38 by tischmid          #+#    #+#             */
/*   Updated: 2023/11/22 14:38:06 by tischmid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_deque	*array_list_to_deque(char **array_list)
{
	t_deque	*deque;

	deque = deque_init();
	while (*array_list)
	{
		deque_push_value_bottom(deque, (t_deque_type)ft_atoi(*array_list));
		++array_list;
		deque->size += 1;
	}
	return (deque);
}
