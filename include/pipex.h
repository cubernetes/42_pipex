/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 11:45:54 by tosuman           #+#    #+#             */
/*   Updated: 2024/01/17 21:21:13 by tosuman          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>

# define HEREDOC_PATH "/tmp/.pipex_heredoc_unlink_me"
# define PS2 "> "
# define HEREDOC_QUALIFIER "here_doc"

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
	int		inerrno;
	int		outerrno;
	char	*heredoc_delim;
	size_t	n_cmds;
}			t_pipeline;

void		initialize_pipeline(int argc, char **argv, char **envp,
				t_pipeline *pipeline);
char		*which(char **av, t_pipeline pipeline);
void		help_and_exit(int argc, char **argv, int fd);
void		file_error(t_pipeline *pipeline, char *file_path, int exit_code);
char		*ft_dirname(char *path);
void		finalize_pipeline(t_pipeline *pipeline);
void		free_strarr(char **arrlist);
int			open_input(t_pipeline *pipeline);
int			open_output(t_pipeline *pipeline);

#endif
