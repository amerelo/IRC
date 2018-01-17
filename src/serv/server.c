/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 05:11:29 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:13:00 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	srv_create(t_env *e, int port)
{
	int					server_socket;
	int					option_true;
	struct sockaddr_in	serv;

	if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		usage("socket fail");
	ft_memset((char *)&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	option_true = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option_true,
		sizeof(option_true));
	if (bind(server_socket, (struct sockaddr*)&serv, sizeof(serv)) == -1)
		usage("socket fail");
	if (listen(server_socket, 42) == -1)
		usage("listen fail");
	e->fds[server_socket].type = FD_SERV;
	e->fds[server_socket].fct_read = srv_accept;
}

void	system_gmsg(t_env *e, int cs, char *sms)
{
	int i;

	i = 0;
	while (i < e->maxfd)
	{
		if (i != cs && e->fds[i].client == 1 && (e->fds[i].chan &&
		e->fds[cs].chan == e->fds[i].chan))
		{
			send_to_client(sms, i, GMSG, e->fds[cs].name);
		}
		i++;
	}
}

void	srv_accept(t_env *e, int s)
{
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	if ((cs = accept(s, (struct sockaddr*)&csin, &csin_len)) == -1)
		ft_putendl("accept error");
	clean_fd(&e->fds[cs]);
	e->fds[cs].type = FD_CLIENT;
	e->fds[cs].fct_read = read_client;
	ft_strncpy(e->fds[cs].name, "NONE", 4);
	e->fds[cs].name[4] = '\0';
	e->fds[cs].chan = NULL;
	e->fds[cs].client = 1;
	ft_putendl("New client");
}

void	write_in_buf(t_env *e, int cs, char **tab)
{
	int		i;
	int		y;

	y = 2;
	while (tab[y])
	{
		i = 0;
		while (tab[y][i])
		{
			if (e->fds[cs].msg.write == BUF_T)
				e->fds[cs].msg.write = 0;
			e->fds[cs].msg.buf_t[e->fds[cs].msg.write++] = tab[y][i++];
		}
		e->fds[cs].msg.buf_t[e->fds[cs].msg.write++] = ' ';
		y++;
	}
	e->fds[cs].msg.write--;
	e->fds[cs].msg.buf_t[e->fds[cs].msg.write] = '\0';
}
