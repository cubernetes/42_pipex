/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_push_node_bottom.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 14:42:03 by tischmid          #+#    #+#             */
/*   Updated: 2023/11/22 14:42:08 by tischmid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	deque_push_node_bottom(t_deque *deque, t_deque_node *node)
{
	if (deque->head)
	{
		node->next = deque->head;
		node->prev = deque->head->prev;
		deque->head->prev->next = node;
		deque->head->prev = node;
	}
	else
	{
		deque->head = node;
		deque->head->next = deque->head;
		deque->head->prev = deque->head;
	}
	deque->size += 1;
}
