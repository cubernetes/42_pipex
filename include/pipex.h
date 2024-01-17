/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tosuman <timo42@proton.me>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 11:45:54 by tosuman           #+#    #+#             */
/*   Updated: 2024/01/17 20:39:58 by tosuman          ###   ########.fr       */
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

#endif
