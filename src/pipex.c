/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 02:14:40 by tosuman           #+#    #+#             */
/*   Updated: 2024/01/17 21:04:18 by tosuman          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../include/pipex.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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

int	open_input(t_pipeline *pipeline)
{
	if (!access(pipeline->infile_path, R_OK))
		return (open(pipeline->infile_path, O_RDONLY));
	file_error(pipeline, pipeline->infile_path, 1);
	pipeline->inerrno = 1;
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
		(free(dirname), dirname = ft_strjoin(tmp, "/"), free(tmp));
	}
	return (free_strarr(orig_split_path), dirname);
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

void	finalize_pipeline(t_pipeline *pipeline)
{
	if (pipeline->in_fd != -1)
		close(pipeline->in_fd);
	if (pipeline->out_fd != -1)
		close(pipeline->out_fd);
	if (pipeline->heredoc_delim)
		unlink(pipeline->infile_path);
	free(pipeline->cmds);
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
		if (!access(full_path, X_OK | R_OK | F_OK))
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

/* no command parsing is done save for spliting by spaces (quotes don't work) */
/* printf("[\033[32mDEBUG\033[m] CMD \033[31m%s\033[m: "
	"EXECVE'ing the following (pid: %d)\n", *nm, getpid()); */
/* printf("[\033[32mDEBUG\033[m] CMD \033[31m%s\033[m: "
	"%s, %s\n", *nm, path, av[1]); */
void	execute_child(char **nm, t_pipeline *pipeline, int (*fds)[2][2], int *n)
{
	char	**av;
	char	*path;

	av = ft_split(*nm, ' ');
	path = which(av, *pipeline);
	if (*n == 1)
		dup2(pipeline->in_fd, STDIN_FILENO);
	else
		dup2((*fds)[(*n + 1) & 1][0], STDIN_FILENO);
	if (!nm[1])
		dup2(pipeline->out_fd, STDOUT_FILENO);
	else
		dup2((*fds)[*n & 1][1], STDOUT_FILENO);
	if (pipeline->in_fd != -1)
		close(pipeline->in_fd);
	if (pipeline->out_fd != -1)
		close(pipeline->out_fd);
	(close((*fds)[*n & 1][0]), close((*fds)[*n & 1][1]));
	(close((*fds)[(*n + 1) & 1][0]), close((*fds)[(*n + 1) & 1][1]));
	if (execve(path, av, pipeline->envp) < 0)
		exit(5);
}

/* printf("[\033[32mDEBUG\033[m] CMD \033[31m%s\033[m: CONTINUING\n",
	*_cmds); */
/* printf("[\033[32mDEBUG\033[m] CMD \033[31m%s\033[m: EXECVE'ing\n",
	*_cmds); */
pid_t	*pipexecve(t_pipeline *pipeline, int (*fds)[2][2], int *n)
{
	int		pid;
	pid_t	*pids;
	char	**_cmds;

	*n = 0;
	pids = malloc(sizeof(*pids) * (pipeline->n_cmds + 1));
	(pipe((*fds)[*n & 1]), pipe((*fds)[(*n + 1) & 1]));
	_cmds = pipeline->cmds - 1;
	while (++*n && *++_cmds)
	{
		if ((*n == 1 && pipeline->inerrno) || (!_cmds[1]
				&& pipeline->outerrno))
			continue ;
		pid = fork();
		if (pid == 0 && (free(pids), 1))
			execute_child(_cmds, pipeline, fds, n);
		if ((close((*fds)[(*n + 1) & 1][0]) < 0 || close((*fds)[(*n + 1)
			& 1][1]) < 0) && ft_dprintf(2, "%s: %s: %s\n",
			pipeline->pipex_name, "error closing file descriptor"))
			exit(7);
		pipe((*fds)[(*n + 1) & 1]);
		pids[*n - 1] = pid;
	}
	return (pids[*n - 1] = -1, pids);
}

int	pipex(t_pipeline pipeline)
{
	pid_t	*pids;
	int		fds[2][2];
	int		status;
	int		n;

	pids = pipexecve(&pipeline, &fds, &n);
	if (pipeline.in_fd != -1)
		close(pipeline.in_fd);
	if (pipeline.out_fd != -1)
		close(pipeline.out_fd);
	(close(fds[0][0]), close(fds[0][1]), close(fds[1][0]), close(fds[1][1]));
	(free(NULL), status = 0, n = -1);
	while (++n < (int)pipeline.n_cmds)
	{
		if (n == 0 && pipeline.inerrno)
			status = pipeline.inerrno;
		else if (n == (int)pipeline.n_cmds - 1 && pipeline.outerrno)
			status = pipeline.outerrno;
		else if (waitpid(pids[n], pids, 0) == -1)
			(ft_dprintf(2, "%s: %d: %s: %s\n", pipeline.pipex_name, status,
					"error in child process", pipeline.cmds[n]), exit(status));
		else if (WIFEXITED(pids[n]))
			status = WEXITSTATUS(pids[n]);
	}
	return (free(pids), status);
}

/* pipex with debug statements (printf, not ft_printf, for better buffering)*/
/*
int	pipex(t_pipeline pipeline)
{
	pid_t	*pids;
	int		fds[2][2];
	int		status;
	int		n;
	int		val;
	int		val2;

	pids = pipexecve(&pipeline, &fds, &n);
	if (pipeline.in_fd != -1)
		close(pipeline.in_fd);
	if (pipeline.out_fd != -1)
		close(pipeline.out_fd);
	(close(fds[0][0]), close(fds[0][1]), close(fds[1][0]), close(fds[1][1]));
	(free(NULL), status = 0, n = -1);
	sleep(1);
	printf("[\033[32mDEBUG\033[m] \t\033[41;30m STATUS STARTS WITH %d \033[m\n",
		status);
	while (++n < (int)pipeline.n_cmds)
	{
		printf("[\033[32mDEBUG\033[m] LOOP for CMD: \033[31m%s\033[m\n",
			pipeline.cmds[n]);
		if (n == 0 && pipeline.inerrno && printf("[\033[32mDEBUG\033[m] \tFILE "
				"READ ERROR, \033[41;30m SETTING THE STATUS TO %d \033[m\n",
				pipeline.inerrno))
			status = pipeline.inerrno;
		else if (n == (int)pipeline.n_cmds - 1 && pipeline.outerrno
			&& printf("[\033[32mDEBUG\033[m] \tFILE WRITE ERROR, \033[41;30m "
			"SETTING THE STATUS TO %d \033[m\n", pipeline.outerrno))
			status = pipeline.outerrno;
		else if (printf("[\033[32mDEBUG\033[m] \tWAITING for PID %d...\n",
			pids[n]) && waitpid(pids[n], pids, 0) != -1
			&& printf("[\033[32mDEBUG\033[m] \tWAITING DONE\n"))
		{
			printf("[\033[32mDEBUG\033[m] \tDID IT EXIT NORMALLY?: ");
			val = WIFEXITED(pids[n]);
			if (val && printf("\033[42;30m YES (VALUE WAS %d)! \033[m\n", val))
			{
				status = WEXITSTATUS(pids[n]);
				printf("[\033[32mDEBUG\033[m] \t SETTING THE STATUS TO %d \n",
					status);
			}
			else
				printf("\033[41;30m NO (VALUE WAS %d) (NOT SETTING THE "
					"STATUS!) \033[m\n", val);
			printf("[\033[32mDEBUG\033[m] \tVALUE OF WEXITSTATUS(pids[n]) is "
				"this: %d\n", WEXITSTATUS(pids[n]));
			printf("[\033[32mDEBUG\033[m] \tAND DID IT RECEIVE A SIGNAL?: ");
			val2 = WIFSIGNALED(pids[n]);
			if (val2 && printf("\033[42;30m YES (VALUE WAS %d)! \033[m\n",
				val2))
				printf("[\033[32mDEBUG\033[m] \tSIGNAL FOR PID %d WAS %d\n",
					pids[n], WTERMSIG(pids[n]));
			else
				printf("\033[41;30m NO (VALUE WAS %d) \033[m\n", val2);
		}
		else
			(ft_dprintf(2, "%s: %d: %s: %s\n", pipeline.pipex_name, status,
					"error in child process", pipeline.cmds[n]), exit(status));
	}
	return (free(pids), status);
}
*/

/* printf("[\033[32mDEBUG\033[m] FINAL EXIT STATUS: \033[41;30m %d \033[m\n",
	status); */
int	main(int argc, char **argv, char **envp)
{
	t_pipeline	pipeline;
	int			status;

	initialize_pipeline(argc, argv, envp, &pipeline);
	status = pipex(pipeline);
	finalize_pipeline(&pipeline);
	return (status);
}
