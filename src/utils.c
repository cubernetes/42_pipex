/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 21:08:57 by tosuman           #+#    #+#             */
/*   Updated: 2024/01/20 15:00:04 by tosuman          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _POSIX_C_SOURCE 200809L
#include "../include/pipex.h"
#include "../libft/libft.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

char	*ft_dirname(char *path)
{
	char	**split_path;
	char	**orig_split_path;
	char	*dirname;
	char	*tmp;
	int		len;

	if (!path || !path[0])
		return (NULL);
	split_path = ft_split(path, '/');
	orig_split_path = split_path;
	len = 0;
	while (split_path[len])
		++len;
	if (len == 0)
		return (free_strarr(split_path), ft_strdup("/"));
	if (path[0] == '/')
		dirname = ft_strdup("/");
	else
		dirname = ft_strdup("./");
	while (--len)
	{
		tmp = ft_strjoin(dirname, *split_path++);
		(free(dirname), dirname = ft_strjoin(tmp, "/"), free(tmp));
	}
	return (free_strarr(orig_split_path), dirname);
}

static char	*search_executable(char *program, char **path_parts)
{
	char	*path;
	char	*full_path;

	if (ft_strchr(program, '/'))
		return (program);
	full_path = NULL;
	while (program && *program && *path_parts)
	{
		path = ft_strjoin(*path_parts, "/");
		full_path = ft_strjoin(path, program);
		free(path);
		if (!access(full_path, X_OK | R_OK | F_OK)
			&& open(full_path, O_DIRECTORY) == -1)
			break ;
		free(full_path);
		full_path = NULL;
		++path_parts;
	}
	return (full_path);
}

char	*which(char **av, t_pipeline pipeline)
{
	char	**path_parts;
	char	*full_path;

	path_parts = NULL;
	while (*pipeline.envp)
	{
		if (!ft_strncmp(*pipeline.envp, "PATH=", 5))
			path_parts = ft_split(*pipeline.envp + 5, ':');
		++pipeline.envp;
	}
	if (!path_parts)
	{
		ft_dprintf(2, "%s: %s: %s\n", pipeline.pipex_name, av[0],
			"No such file or directory");
		(finalize_pipeline(&pipeline), free_strarr(av), exit(127));
	}
	full_path = search_executable(av[0], path_parts);
	free_strarr(path_parts);
	if (!full_path)
	{
		ft_dprintf(2, "%s: %s: %s\n", pipeline.pipex_name, av[0],
			"command not found");
		(finalize_pipeline(&pipeline), free_strarr(av), exit(127));
	}
	return (full_path);
}
