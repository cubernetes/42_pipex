/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 21:09:31 by tosuman           #+#    #+#             */
/*   Updated: 2024/01/17 21:18:04 by tosuman          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"
#include "../libft/libft.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void	help_and_exit(int argc, char **argv, int fd)
{
	char	*program;

	(void)argc;
	if (!argv || !argv[0])
		program = "pipex";
	else
		program = argv[0];
	ft_dprintf(fd, "USAGE:\n");
	ft_dprintf(fd, "    %s FILE COMMAND... FILE\n", program);
	ft_dprintf(fd, "    %s here_doc DELIMITER COMMAND... FILE\n", program);
	ft_dprintf(fd, "\n");
	ft_dprintf(fd, "EXAMPLES:\n");
	ft_dprintf(fd, "    %s file1 sort nl file2\n", program);
	ft_dprintf(fd, "    printf 'abc\\nline2\\nwhatever' | %s /dev/stdin sort nl"
		" 'sed s/\\s/_/g' /dev/stdout\n", program);
	exit(1);
}

void	file_error(t_pipeline *pipeline, char *file_path, int exit_code)
{
	ft_dprintf(2, "%s: %s: %s\n",
		pipeline->pipex_name,
		file_path,
		strerror(errno));
	(void)exit_code;
}

/* finalize_pipeline(pipeline); */
/* exit(exit_code); */
void	print_pipeline(t_pipeline pipeline)
{
	int	i;

	ft_printf("pipex name:         %s\n", pipeline.pipex_name);
	ft_printf("infile:             %s\n", pipeline.infile_path);
	ft_printf("infile fd:          %d\n", pipeline.in_fd);
	i = 0;
	while (*pipeline.cmds)
		ft_printf("cmd%d:               %s\n", ++i, *pipeline.cmds++);
	ft_printf("outfile:            %s\n", pipeline.outfile_path);
	ft_printf("outfile fd:         %d\n", pipeline.out_fd);
	ft_printf("append to outfile:  %d\n", pipeline.append);
	ft_printf("number of commands: %d\n", pipeline.n_cmds);
}

void	free_strarr(char **arrlist)
{
	char	**orig;

	orig = arrlist;
	while (*arrlist)
		free(*arrlist++);
	free(orig);
}
