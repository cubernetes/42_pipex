/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 02:14:40 by tosuman           #+#    #+#             */
/*   Updated: 2024/01/17 21:10:52 by tosuman          ###   ########.fr       */
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
