/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tischmid <tischmid@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/22 14:36:56 by tischmid          #+#    #+#             */
/*   Updated: 2023/11/22 14:37:11 by tischmid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

t_deque	*deque_init(void)
{
	t_deque	*deque;

	deque = malloc(sizeof(*deque));
	deque->head = NULL;
	deque->size = 0;
	return (deque);
}
