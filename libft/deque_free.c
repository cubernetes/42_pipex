/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 14:37:19 by tischmid          #+#    #+#             */
/*   Updated: 2023/11/22 14:37:29 by tischmid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

void	deque_free(t_deque *deque)
{
	t_deque_node	*head;
	t_deque_node	*tail;

	head = deque->head;
	if (!head)
		return ;
	tail = head->prev;
	while (head != tail)
	{
		head = head->next;
		free(head->prev);
	}
	free(head);
	free(deque);
}
