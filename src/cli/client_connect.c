/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_connect.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:36:58 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:45:24 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

int		connect_client(char *addr, int port, t_cli *cli)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;
	struct hostent		*he;

	if ((proto = getprotobyname("tcp")) == 0)
		return (-1);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	if ((he = gethostbyname(addr)) == NULL)
	{
		ft_putendl("gethostbyname error");
		return (-1);
	}
	sin.sin_addr.s_addr = inet_addr(
		inet_ntoa(*((struct in_addr **)he->h_addr_list)[0]));
	if (connect(sock, (const struct sockaddr*)&sin, sizeof(sin)) == -1)
	{
		ft_putendl("Connect error");
		return (cli->fds[1]);
	}
	cli->info.connected = 1;
	ft_putendl("success connection please select your nick name to continue");
	return (sock);
}

void	ft_sendconnect(char *buf, t_cli *cli)
{
	char **tab;
	char *tmp;

	if (cli->info.connected)
	{
		close(cli->fds[1]);
		cclean_fd(cli);
	}
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 3) != 0)
	{
		ft_putendl("Connect need [machine] [port]");
		free_tab(tab);
		return ;
	}
	cli->port = ft_atoi(tab[2]);
	cli->fds[1] = connect_client(tab[1], cli->port, cli);
	free_tab(tab);
}
