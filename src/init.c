/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 21:06:46 by tosuman           #+#    #+#             */
/*   Updated: 2024/01/17 21:20:47 by tosuman          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"
#include "../libft/libft.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

/* if we were allowed to use isatty, we would not print PS2 in certain cases */
/* last get_next_line call (that must fail) is to clear the static buffer */
char	*create_heredoc_file(t_pipeline *pipeline)
{
	char	*line;
	int		heredoc_fd;
	size_t	heredoc_sep_len;

	heredoc_fd = open(HEREDOC_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (heredoc_fd < 0)
		file_error(pipeline, HEREDOC_PATH, 8);
	line = NULL;
	heredoc_sep_len = ft_strlen(pipeline->heredoc_delim);
	while (!line || ft_strncmp(line, pipeline->heredoc_delim, heredoc_sep_len))
	{
		if (line)
			ft_dprintf(heredoc_fd, "%s", line);
		ft_dprintf(1, PS2);
		free(line);
		line = get_next_line(0);
	}
	free(line);
	return (HEREDOC_PATH);
}
/* close(0); */
/* get_next_line(0); */

t_pipeline	parse_args(int argc, char **argv)
{
	t_pipeline	pipeline;

	if (argc < 4)
		help_and_exit(argc, argv, STDOUT_FILENO);
	pipeline.pipex_name = argv[0];
	pipeline.append = 0;
	pipeline.heredoc_delim = NULL;
	if (ft_streq(argv[1], HEREDOC_QUALIFIER))
	{
		pipeline.append = 1;
		pipeline.heredoc_delim = argv[2];
		pipeline.infile_path = create_heredoc_file(&pipeline);
		++argv;
		--argc;
	}
	else
		pipeline.infile_path = argv[1];
	argv += 2;
	pipeline.cmds = malloc(sizeof(*pipeline.cmds) * (size_t)(argc - 1));
	pipeline.outfile_path = argv[argc - 3];
	argv[argc - 3] = NULL;
	pipeline.n_cmds = (size_t)argc - 3;
	while (--argc >= 1)
		pipeline.cmds[argc - 1] = argv[argc - 1];
	return (pipeline);
}

void	initialize_pipeline(int argc, char **argv,
		char **envp, t_pipeline *pipeline)
{
	*pipeline = parse_args(argc, argv);
	pipeline->inerrno = 0;
	pipeline->outerrno = 0;
	pipeline->in_fd = open_input(pipeline);
	pipeline->out_fd = open_output(pipeline);
	pipeline->envp = envp;
}

int	open_input(t_pipeline *pipeline)
{
	if (!access(pipeline->infile_path, R_OK))
		return (open(pipeline->infile_path, O_RDONLY));
	file_error(pipeline, pipeline->infile_path, 1);
	pipeline->inerrno = 1;
	return (-1);
}

int	open_output(t_pipeline *pipeline)
{
	char	*dirname;

	dirname = ft_dirname(pipeline->outfile_path);
	if (dirname && (!access(pipeline->outfile_path, W_OK)
			|| !access(dirname, W_OK)))
	{
		free(dirname);
		if (pipeline->append)
			return (open(pipeline->outfile_path,
					O_CREAT | O_WRONLY | O_APPEND, 0644));
		else
			return (open(pipeline->outfile_path,
					O_CREAT | O_WRONLY | O_TRUNC, 0644));
	}
	free(dirname);
	file_error(pipeline, pipeline->outfile_path, 1);
	pipeline->outerrno = 1;
	return (-1);
}
