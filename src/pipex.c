/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 02:14:40 by tosuman           #+#    #+#             */
/*   Updated: 2023/12/06 11:47:30 by tosuman          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define HEREDOC_PATH "/tmp/.pipex_heredoc_unlink_me"
#define PS2 "> "

typedef struct s_pipeline
{
	char	**cmds;
	char	**envp;
	char	*pipex_name;
	char	*outfile_path;
	char	*infile_path;
	int		out_fd;
	int		in_fd;
	int		append;
	char	*heredoc_delim;
}			t_pipeline;

void	finalize_pipeline(t_pipeline *pipeline);

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
	ft_dprintf(fd, "    printf 'abc\\nline2\\nwhatever' | %s /dev/stdin sort nl "
		"'sed s/\\s/_/g' /dev/stdout\n", program);
	exit(1);
}

void	file_error(t_pipeline *pipeline, char *file_path)
{
	ft_dprintf(2, "%s: %s: %s\n",
		pipeline->pipex_name,
		file_path,
		strerror(errno)),
	finalize_pipeline(pipeline);
	exit(2);
}

char	*create_heredoc_file(t_pipeline *pipeline)
{
	char	*line;
	int		heredoc_fd;
	size_t	heredoc_sep_len;

	heredoc_fd = open(HEREDOC_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (heredoc_fd < 0)
		file_error(pipeline, HEREDOC_PATH);
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
	close(0);
	get_next_line(0);
	return (HEREDOC_PATH);
}

t_pipeline	parse_args(int argc, char **argv)
{
	t_pipeline	pipeline;

	if (argc < 4)
		help_and_exit(argc, argv, STDOUT_FILENO);
	pipeline.pipex_name = argv[0];
	pipeline.append = 0;
	pipeline.heredoc_delim = NULL;
	if (!ft_strcmp(argv[1], "here_doc"))
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
	while (--argc >= 1)
		pipeline.cmds[argc - 1] = argv[argc - 1];
	return (pipeline);
}

void	print_pipeline(t_pipeline pipeline)
{
	ft_printf("pipex name:                  %s\n", pipeline.pipex_name);
	ft_printf("infile:                      %s\n", pipeline.infile_path);
	ft_printf("infile fd:                   %d\n", pipeline.in_fd);
	while (*pipeline.cmds)
		ft_printf("cmd:                         %s\n", *pipeline.cmds++);
	ft_printf("outfile:                     %s\n", pipeline.outfile_path);
	ft_printf("outfile fd:                  %d\n", pipeline.out_fd);
	ft_printf("append to outfile:           %d\n", pipeline.append);
}

int	open_input(t_pipeline *pipeline)
{
	if (!access(pipeline->infile_path, R_OK))
		return (open(pipeline->infile_path, O_RDONLY));
	file_error(pipeline, pipeline->infile_path);
	return (-1);
}

void	free_strarr(char **arrlist)
{
	char	**orig;

	orig = arrlist;
	while (*arrlist)
		free(*arrlist++);
	free(orig);
}

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
		free(dirname);
		dirname = ft_strjoin(tmp, "/");
		free(tmp);
	}
	free_strarr(orig_split_path);
	return (dirname);
}

int	open_output(t_pipeline *pipeline)
{
	char	*dirname;

	dirname = ft_dirname(pipeline->outfile_path);
	if (dirname && (!access(pipeline->outfile_path, W_OK) || !access(dirname, W_OK)))
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
	file_error(pipeline, pipeline->outfile_path);
	return (-1);
}

void	finalize_pipeline(t_pipeline *pipeline)
{
	close(pipeline->in_fd);
	close(pipeline->out_fd);
	if (pipeline->heredoc_delim)
		unlink(pipeline->infile_path);
	free(pipeline->cmds);
}

void	initialize_pipeline(int argc, char **argv,
		char **envp, t_pipeline *pipeline)
{
	*pipeline = parse_args(argc, argv);
	pipeline->in_fd = 0;
	pipeline->out_fd = 1;
	pipeline->in_fd = open_input(pipeline);
	pipeline->out_fd = open_output(pipeline);
	pipeline->envp = envp;
	/* print_pipeline(*pipeline); */
}

static char	*search_executable(char *program, char **path_parts)
{
	char	*path;
	char	*full_path;

	full_path = NULL;
	while (program && *program && *path_parts)
	{
		path = ft_strjoin(*path_parts, "/");
		full_path = ft_strjoin(path, program);
		free(path);
		if (!access(full_path, X_OK | R_OK | F_OK))
			break ;
		free(full_path);
		full_path = NULL;
		++path_parts;
	}
	return (full_path);
}

char	*which(char *program, t_pipeline pipeline)
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
		ft_dprintf(2, "%s: %s: %s\n", pipeline.pipex_name, program,
			"No such file or directory");
		(finalize_pipeline(&pipeline), exit(3));
	}
	full_path = search_executable(program, path_parts);
	free_strarr(path_parts);
	if (!full_path)
	{
		ft_dprintf(2, "%s: %s: %s\n", pipeline.pipex_name, program,
			"command not found");
		(finalize_pipeline(&pipeline), exit(4));
	}
	return (full_path);
}

void	execute_child(t_pipeline *pipeline, int (*fds)[2][2], int *n)
{
	char	**av;
	char	*path;

	av = ft_split(*pipeline->cmds, ' ');
	path = which(av[0], *pipeline);
	if (*n == 1)
		dup2(pipeline->in_fd, STDIN_FILENO);
	else
		dup2((*fds)[(*n + 1) & 1][0], STDIN_FILENO);
	if (!pipeline->cmds[1])
		dup2(pipeline->out_fd, STDOUT_FILENO);
	else
		dup2((*fds)[*n & 1][1], STDOUT_FILENO);
	(close((*fds)[*n & 1][0]), close((*fds)[*n & 1][1]));
	(close((*fds)[(*n + 1) & 1][0]), close((*fds)[(*n + 1) & 1][1]));
	if (execve(path, av, pipeline->envp) < 0)
		exit(5);
}

void	pipexecve(t_pipeline *pipeline, int (*fds)[2][2], int *n)
{
	int		pid;

	*n = 0;
	(pipe((*fds)[*n & 1]), pipe((*fds)[(*n + 1) & 1]));
	while (++*n && *pipeline->cmds)
	{
		pid = fork();
		if (pid == 0)
			execute_child(pipeline, fds, n);
		if ((close((*fds)[(*n + 1) & 1][0]) < 0 || close((*fds)[(*n + 1)
			& 1][1]) < 0) && ft_dprintf(2, "%s: %s: %s\n",
			pipeline->pipex_name, "error closing file descriptor"))
			exit(7);
		pipe((*fds)[(*n + 1) & 1]);
		++pipeline->cmds;
	}
}

void	pipex(t_pipeline pipeline)
{
	/* int		wstatus; */
	int		fds[2][2];
	int		n;

	pipexecve(&pipeline, &fds, &n);
	(close(pipeline.in_fd), close(pipeline.out_fd));
	(close(fds[n & 1][0]), close(fds[n & 1][1]));
	(close(fds[(n + 1) & 1][0]), close(fds[(n + 1) & 1][1]));
	/* if (waitpid(pid, &wstatus, 0) < 0) */
		/* (ft_dprintf(2, "%s: %d: %s: %s\n", pipeline.pipex_name, wstatus, */
				/* "error in child process", *pipeline.cmds), exit(6)); */
}

int	main(int argc, char **argv, char **envp)
{
	t_pipeline	pipeline;

	initialize_pipeline(argc, argv, envp, &pipeline);
	pipex(pipeline);
	finalize_pipeline(&pipeline);
	return (0);
}
