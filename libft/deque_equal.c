/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_equal.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 14:24:11 by tischmid          #+#    #+#             */
/*   Updated: 2023/11/22 14:50:52 by tischmid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	deque_equal(t_deque *deque_a, t_deque *deque_b)
{
	t_deque_node	*head_a;
	t_deque_node	*head_b;

	if (!deque_a->head && !deque_b->head)
		return (1);
	if ((!deque_a->head && deque_b->head)
		|| (deque_a->head && !deque_b->head))
		return (0);
	head_a = deque_a->head->next;
	head_b = deque_b->head->next;
	while (head_a != deque_a->head && head_b != deque_b->head)
	{
		if (head_a->data != head_b->data)
			return (0);
		head_a = head_a->next;
		head_b = head_b->next;
	}
	if (head_a == deque_a->head && head_b == deque_b->head)
		return (1);
	ft_printf("THIS\n");
	return (0);
}
