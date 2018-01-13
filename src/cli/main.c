/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 06:34:59 by amerelo           #+#    #+#             */
/*   Updated: 2017/12/12 03:04:15 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	usage(char *str)
{
	ft_putstr(str);
	ft_putendl(" client");
	exit(2);
}

int		connect_client(char *addr, int port, t_cli *cli)
{
	int					sock;
	struct protoent		*proto;
	struct sockaddr_in	sin;

	proto = getprotobyname("tcp");
	if (proto == 0)
		return (-1);
	sock = socket(PF_INET, SOCK_STREAM, proto->p_proto);

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(addr);
	if (connect(sock, (const struct sockaddr*)&sin, sizeof(sin)) == -1)
	{
		ft_putendl("Connect error");
		return (cli->fds[1]);
	}
	cli->info.connected = 1;
	return (sock);
}

void	init_fdc(t_cli *cli)
{
	cli->max = 0;
	FD_ZERO(&cli->fd_read);

	ft_bzero(cli->buf, BUF_SIZE + 1);
	FD_SET(cli->fds[0], &cli->fd_read);
	if (cli->info.connected)
	{
		FD_SET(cli->fds[1], &cli->fd_read);
		cli->max = cli->fds[1];
	}
	else
		cli->max = cli->fds[0];
}

void	send_packet(t_cli *cli, t_sms *sms)
{
	size_t	start;

	start = 0;
	while (start < sizeof(*sms))
	{
		printf("start: %lu - %lu\n", start, sizeof(*sms));
		unsigned char *send_addr = ((unsigned char *)sms) + start;
		send(cli->fds[1], send_addr, BUF_SIZE, 0);
		start += BUF_SIZE;
	}
}

void	send_to_serv(char* txt, t_cli *cli, enum types test)
{
	size_t	offset;
	char	buff[SMS_SIZE + 1];
	t_sms	sms;

	sms.header.mytype = test;
	offset = 0;
	while (ft_strlen(txt) >= offset)
	{
		ft_strncpy(buff, txt + offset, SMS_SIZE);
		buff[SMS_SIZE] = 0;
		ft_memcpy(sms.sms, buff, SMS_SIZE + 1);
		send_packet(cli, &sms);
		offset += SMS_SIZE;
	}
}

void	free_tab(char **tab)
{
	int i;

	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

int		verif_narg(char **tab, int limit)
{
	int i;

	i = 0;
	while (tab[i])
	{
		if (i >= limit)
			return (1);
		i++;
	}
	return ((i != limit) ? 1 : 0);
}

void	ft_connect(char *buf, t_cli *cli)
{
	char **tab;
	char *tmp;

	ft_putendl(buf);
	if (cli->info.connected)
	{
		ft_putendl("already connected");
		return ;
	}
	tmp = ft_strrchr(buf, '\n');
	*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 3))
	{
		ft_putendl("connect need [machine] [port]");
		free_tab(tab);
		return ;
	}
	cli->port = ft_atoi(tab[2]);
	cli->fds[1] = connect_client(tab[1], cli->port, cli);
	free_tab(tab);
}

void	ft_nick(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	tmp = ft_strrchr(buf, '\n');
	*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) || (ft_strlen(tab[1]) > NAME_SIZE))
	{
		ft_putstr("Nick need [nick-name]-");
		ft_putnbr(NAME_SIZE);
		ft_putendl("-maxsize");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, NICK);
	free_tab(tab);
}

void	ft_join(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	tmp = ft_strrchr(buf, '\n');
	*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) || (ft_strlen(tab[1]) > NAME_SIZE))
	{
		ft_putstr("Join need [room-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, JOIN);
	free_tab(tab);
}

void	ft_leave(char *buf, t_cli *cli)
{
	send_to_serv(buf, cli, LEAVE);
}

void	ft_msg(char *buf, t_cli *cli)
{
	// pars buf
	send_to_serv(buf, cli, MSG);
}

void	ft_gmsg(char *buf, t_cli *cli)
{
	if (cli->info.connected)
		send_to_serv(buf, cli, GMSG);
}

void	execut_command(char *cmd, t_cli *cli)
{
	int i;
	static t_cmd tab[] = {
		{"/connect", &ft_connect}, {"/nick", &ft_nick},
		{"/join", &ft_join}, {"/leave", &ft_leave},
		{"NONE", NULL}
	};

	i = 0;
	while (ft_strncmp(tab[i].c, "NONE", 4) != 0)
	{
		if (ft_strncmp(cmd, tab[i].c, ft_strlen(tab[i].c)) == 0)
		{
			tab[i].cmds(cmd, cli);
			return ;
		}
		i++;
	}
	if (cmd[0] == '/')
		ft_putendl("command not fund");
	else
		ft_gmsg(cmd, cli);
}

void	read_std_e(t_cli *cli)
{
	int			r;
	static char	buf[BUF_E_READ +1];

	ft_bzero(buf, BUF_E_READ +1);
	while ((r = read(1, buf, BUF_E_READ)) > 0)
	{
		execut_command(buf, cli);
		if (ft_strchr(buf, '\n'))
			return ;
		ft_bzero(buf, BUF_E_READ +1);
	}
}

void	resive_srv(t_cli *cli)
{
	int		r;
	char	buf[BUF_SIZE +1];

	// need to put a while in order to get all message in one try ?
	if ((r = recv(cli->fds[1], buf, BUF_SIZE, 0)) > 0)
	{
		buf[r] = '\0';
		ft_putstr(buf);
	}
	else
	{
		close(cli->fds[1]);
		// clean_fd(&e->fds[cs]);
		ft_putendl("Server Error");
		exit(2);
	}
}

void	check_fdc(t_cli *cli)
{
	if (FD_ISSET(cli->fds[0], &cli->fd_read))
		read_std_e(cli);
	if (cli->info.connected && FD_ISSET(cli->fds[1], &cli->fd_read))
		resive_srv(cli);
	if (FD_ISSET(cli->fds[0], &cli->fd_read) || (cli->info.connected &&
	FD_ISSET(cli->fds[1], &cli->fd_read)))
		cli->r--;
}

void	cli_loop(t_cli *cli)
{
	while (1)
	{
		init_fdc(cli);
		cli->r = select(cli->max + 1, &cli->fd_read, NULL, NULL, NULL);
		check_fdc(cli);
	}
}

void	init_client(t_cli *cli)
{
	cli->fds[0] = 1;
	cli->fds[1] = 0;
	cli->info.connected = 0;
	ft_strncpy(cli->info.chan, "NONE", 4);
	cli->info.chan[4] = '\0';
	ft_strncpy(cli->info.name, "NONE", 4);
	cli->info.name[4] = '\0';
}

int		main(void)
{
	t_cli	cli;

	init_client(&cli);
	cli_loop(&cli);
	return(0);
}

// void	srv_accept(t_env *e, int s)
// {
// 	int					cs;
// 	struct sockaddr_in	csin;
// 	socklen_t			csin_len;
//
// 	csin_len = sizeof(csin);
// 	if ((cs = accept(s, (struct sockaddr*)&csin, &csin_len)) == -1)
// 		ft_putendl("accept error");
//
// 	ft_putstr("New client ");
// 	ft_putnbr(cs);
// 	ft_putstr("\n");
//
// 	// printf("New client #%d from %s:%d\n", cs,
// 	// inet_ntoa(csin.sin_addr), ntohs(csin.sin_port));
// 	clean_fd(&e->fds[cs]);
// 	e->fds[cs].type = FD_CLIENT;
// 	e->fds[cs].fct_read = client_read;
// }
