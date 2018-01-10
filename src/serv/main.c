/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 06:34:59 by amerelo           #+#    #+#             */
/*   Updated: 2017/12/11 08:06:04 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	usage(char *str)
{
	ft_putendl(str);
	exit(2);
}

void	clean_fd(t_fd *fd)
{
	fd->type = FD_FREE;
	fd->fct_read = NULL;
	// fd->fct_write = NULL;
}

// char		*ft_strjoin_free(char *s1, char *s2, int s)
// {
// 	int		x;
// 	int		y;
// 	char	*sol;
//
// 	x = s1 != NULL ? ft_strlen(s1) : 0;
// 	y = s2 != NULL ? ft_strlen(s2) : 0;
// 	sol = (char *)ft_memalloc(sizeof(char) * (x + y + 1));
// 	x = 0;
// 	while (s1 && s1[x])
// 	{
// 		sol[x] = s1[x];
// 		x++;
// 	}
// 	if (s1 && (s == 1 || s == 3))
// 		free(s1);
// 	y = 0;
// 	while (s2 && s2[y])
// 	{
// 		sol[x + y] = s2[y];
// 		y++;
// 	}
// 	if (s2 && (s == 2 || s == 3))
// 		free(s2);
// 	return (sol);
// }

void	init_env(t_env *e)
{
	int				i;
	struct rlimit	rlp;

	if (getrlimit(RLIMIT_NOFILE, &rlp) == -1)
		ft_putendl("error of getrlimit");

	e->maxfd = rlp.rlim_cur;
	if (!(e->fds = malloc(sizeof(*e->fds) * e->maxfd)))
		ft_putendl("malloc error");

	i = 0;
	while (i < e->maxfd)
	{
		clean_fd(&e->fds[i]);
		e->fds[i].sizeread = 0;
		i++;
	}
}

void	srv_loop(t_env *e)
{
	while (1)
	{
		init_fd(e);
		e->r = select(e->max + 1, &e->fd_read, NULL, NULL, NULL);
		check_fd(e);
	}
}

void	start(int port)
{
	t_env	e;

	init_env(&e);
	srv_create(&e, port);
	srv_loop(&e);
}

void	init_fd(t_env *e)
{
	int	i;

	i = 0;
	e->max = 0;
	FD_ZERO(&e->fd_read);
	// FD_ZERO(&e->fd_write);
	while (i < e->maxfd)
	{
		if (e->fds[i].type != FD_FREE)
		{
			// ft_putendl("init fd");
			// ft_putnbr(i);
			// ft_putendl("");

			ft_bzero(e->fds[i].buf_read, BUF_SIZE + 1);
			FD_SET(i, &e->fd_read);
			// if (ft_strlen(e->fds[i].buf_write) > 0)
			// 	FD_SET(i, &e->fd_write);
			e->max = (e->max > i) ? e->max : i;
		}
		i++;
	}
}

void	client_read(t_env *e, int cs)
{
	int		r;
	size_t	offset;
	t_sms	sms;

	if (e->fds[cs].type != FD_CLIENT)
		return ;

	offset = 0;
	while ((r = recv(cs, ((void *)&sms) + offset, BUF_SIZE, 0)) > 0)
	{
		offset += BUF_SIZE;
		if (offset == sizeof(t_sms))
		{
			ft_putstr(sms.sms);
			send(cs, sms.sms, ft_strlen(sms.sms), 0);
			return ;
		}
	}
	close(cs);
	clean_fd(&e->fds[cs]);
	printf("client #%d gone away\n", cs);
}

void	srv_accept(t_env *e, int s)
{
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	if ((cs = accept(s, (struct sockaddr*)&csin, &csin_len)) == -1)
		ft_putendl("accept error");

	// printf("New client #%d from %s:%d\n", cs,
	// inet_ntoa(csin.sin_addr), ntohs(csin.sin_port));
	clean_fd(&e->fds[cs]);
	e->fds[cs].type = FD_CLIENT;
	e->fds[cs].fct_read = client_read;

	ft_strncpy(e->fds[cs].name, "user00001", NAME_SIZE);
	e->fds[cs].name[NAME_SIZE] = '\0';

	ft_putstr("New client ");
	ft_putendl(e->fds[cs].name);
}

void	check_fd(t_env *e)
{
	int	i;

	i = 0;
	while ((i < e->maxfd) && (e->r > 0))
	{
		if (FD_ISSET(i, &e->fd_read))
		{
			// ft_putendl("\ncall of read");
			e->fds[i].fct_read(e, i);
		}
		// if (FD_ISSET(i, &e->fd_write))
		// 	e->fds[i].fct_write(e, i);
		if (FD_ISSET(i, &e->fd_read)) // || FD_ISSET(i, &e->fd_write)
			e->r--;
		i++;
	}
}

void	srv_create(t_env *e, int port)
{
	int		server_socket;
	struct	sockaddr_in serv;

	if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		usage("socket fail");

	ft_memset((char *) &serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	int option_true = 1;

	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option_true, sizeof(option_true));

	if (bind(server_socket , (struct sockaddr*)&serv, sizeof(serv) ) == -1)
		usage("socket fail");
	if (listen(server_socket, 42) == -1)
		usage("listen fail");
	e->fds[server_socket].type = FD_SERV;
	e->fds[server_socket].fct_read = srv_accept;
}

int		main(int ac, char** av)
{
	if (ac != 2)
		usage("need soket");
	start(ft_atoi(av[1]));
	return(0);
}
