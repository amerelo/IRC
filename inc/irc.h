/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 08:03:15 by amerelo           #+#    #+#             */
/*   Updated: 2017/12/11 08:04:55 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_H
# define IRC_H

# include "libft.h"
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>
// # include <.h>

# define FD_FREE	0
# define FD_SERV	1
# define FD_CLIENT	2
# define BUF_SIZE	4096

typedef struct	s_fd
{
	int		type;
	void	(*fct_read)();
	void	(*fct_write)();
	char	buf_read[BUF_SIZE + 1];
	char	buf_write[BUF_SIZE + 1];
}		t_fd;

typedef struct	s_env
{
	t_fd		*fds;
	int		port;
	int		maxfd;
	int		max;
	int		r;
	fd_set	fd_read;
	fd_set	fd_write;
}		t_env;

void	srv_create(t_env *e, int port);
// void	clean_fd(t_fd *fd);

#endif
