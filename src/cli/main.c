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
	ft_putendl("success connection please select your nick name to continue");
	return (sock);
}

void	clean_fd(t_cli *cli)
{
	cli->type2 = FD_FREE;
	cli->info.connected = 0;
	cli->info.chan = 0;
	ft_bzero(cli->info.name, NAME_SIZE + 1);
	ft_strcpy(cli->info.name, "NONE");
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
	size_t			start;
	unsigned char	*send_addr;

	start = 0;
	while (start < sizeof(*sms))
	{
		send_addr = ((unsigned char *)sms) + start;
		send(cli->fds[1], send_addr, BUF_SIZE, 0);
		start += BUF_SIZE;
	}
}

void	send_to_serv(char *txt, t_cli *cli, enum types type)
{
	size_t	offset;
	char	buff[SMS_SIZE + 1];
	t_sms	sms;

	sms.header.mytype = type;
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
	return (i - limit);
}

int		starter_check(t_cli *cli)
{
	if (!cli->info.connected)
	{
		ft_putendl("Need to connection to server");
		return (1);
	}
	if (ft_strcmp(cli->info.name, "NONE") == 0)
	{
		ft_putendl("Select a nick name");
		return (1);
	}
	return (0);
}

void	ft_sendconnect(char *buf, t_cli *cli)
{
	char **tab;
	char *tmp;

	if (cli->info.connected)
	{
		close(cli->fds[1]);
		clean_fd(cli);
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

void	ft_sendnick(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (!cli->info.connected)
	{
		ft_putendl("Need to connection to server");
		return ;
	}
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > NAME_SIZE) ||
		(ft_strcmp(tab[1], "NONE") == 0 || ft_strcmp(tab[1], "ERROR") == 0))
	{
		ft_putstr("Nick need [nick-name]-");
		ft_putnbr(NAME_SIZE);
		ft_putendl("-maxsize or nick not valid");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, NICK);
	free_tab(tab);
}

void	ft_sendjoin(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (starter_check(cli))
		return ;
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > NAME_SIZE))
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

void	ft_sendleave(char *buf, t_cli *cli)
{
	if (!cli->info.connected)
	{
		ft_putendl("need to connection to server");
		return ;
	}
	send_to_serv(buf, cli, LEAVE);
}

void	ft_sendcreate(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (starter_check(cli))
		return ;
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > ROOM_NAME_SIZE))
	{
		ft_putstr("CREATE need [room-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, CREATE);
	free_tab(tab);
}

void	ft_senddelet(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (starter_check(cli))
		return ;
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > ROOM_NAME_SIZE))
	{
		ft_putstr("DELET need [room-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, DELET);
	free_tab(tab);
}

void	ft_sendlist(char *buf, t_cli *cli)
{
	if (starter_check(cli))
		return ;
	send_to_serv(buf, cli, LIST);
}

void	ft_sendwho(char *buf, t_cli *cli)
{
	if (starter_check(cli) || cli->info.connected == 0)
		return ;
	send_to_serv(buf, cli, WHO);
}

void	ft_sendmsg(char *buf, t_cli *cli)
{
	char	**tab;

	if (ft_strlen(buf) <= 1)
		return ;
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 3) < 0 || (ft_strlen(tab[1]) > NAME_SIZE) ||
		ft_strcmp(tab[1], "NONE") == 0)
	{
		ft_putstr("MSG need [user-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize [msg]");
		free_tab(tab);
		return ;
	}
	send_to_serv(buf, cli, MSG);
	free_tab(tab);
}

void	ft_sendgmsg(char *buf, t_cli *cli)
{
	if (cli->info.connected && ft_strlen(buf) > 1)
		send_to_serv(buf, cli, GMSG);
}

void	execut_command(char *cmd, t_cli *cli)
{
	int				i;
	static t_cmd	tab[] = {
		{"/connect", &ft_sendconnect}, {"/nick", &ft_sendnick},
		{"/join", &ft_sendjoin}, {"/leave", &ft_sendleave},
		{"/create", &ft_sendcreate}, {"/list", &ft_sendlist},
		{"/who", &ft_sendwho}, {"/msg", &ft_sendmsg},
		{"/delet", &ft_senddelet}, {"NONE", NULL}
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
		ft_sendgmsg(cmd, cli);
}

void	read_std_e(t_cli *cli)
{
	int			r;
	static char	buf[BUF_E_READ + 1];

	ft_bzero(buf, BUF_E_READ + 1);
	while ((r = read(1, buf, BUF_E_READ)) > 0)
	{
		if (ft_strchr(buf, '\n'))
		{
			execut_command(buf, cli);
			return ;
		}
		execut_command(buf, cli);
		ft_bzero(buf, BUF_E_READ + 1);
	}
}

void	ft_getnick(t_sms *sms, t_cli *cli)
{
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("Nick name not valid try again");
		return ;
	}
	ft_strcpy(cli->info.name, sms->sms);
	ft_putendl("Valid nick name");
}

void	ft_getjoin(t_sms *sms, t_cli *cli)
{
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("Chat Room not valid try again");
		return ;
	}
	cli->info.chan = 1;
	ft_putendl("Join room");
}

void	ft_getleave(t_sms *sms, t_cli *cli)
{
	(void)sms;
	cli->info.chan = 0;
	ft_putendl("Leave room");
}

void	ft_getcreate(t_sms *sms, t_cli *cli)
{
	(void)cli;
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("Create fail try again");
		return ;
	}
	ft_putstr("Create room ");
	ft_putendl(sms->sms);
}

void	ft_getdelet(t_sms *sms, t_cli *cli)
{
	(void)cli;
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("DELET fail try again");
		return ;
	}
	ft_putstr("DELET room ");
	ft_putendl(sms->sms);
}

void	ft_getlist(t_sms *sms, t_cli *cli)
{
	(void)cli;
	ft_putstr("->Room: ");
	ft_putendl(sms->sms);
}

void	ft_getwho(t_sms *sms, t_cli *cli)
{
	(void)cli;
	ft_putstr("->User: ");
	ft_putendl(sms->sms);
}

void	ft_getgmsg(t_sms *sms, t_cli *cli)
{
	static int	flag = -1;

	(void)cli;
	if (sms->sms[0] == '\0')
		return ;
	if (flag == -1 && ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else if (flag == -1)
		flag = 0;
	if (flag == 1)
	{
		ft_putstr(sms->header.user);
		ft_putstr(": ");
	}
	if (ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else
		flag = 0;
	ft_putstr(sms->sms);
}

void	ft_getmsg(t_sms *sms, t_cli *cli)
{
	static int	flag = -1;

	(void)cli;
	if (sms->sms[0] == '\0')
		return ;
	if (flag == -1 && ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else if (flag == -1)
		flag = 0;
	if (flag == 1)
	{
		ft_putstr("(PRIVATE)");
		ft_putstr(sms->header.user);
		ft_putstr(": ");
	}
	if (ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else
		flag = 0;
	ft_putstr(sms->sms);
}

void	handle_command(t_cli *cli, t_sms *sms)
{
	int				i;
	static t_ccmd	tab[] = {
		{NICK, &ft_getnick}, {JOIN, &ft_getjoin}, {LEAVE, &ft_getleave},
		{CREATE, &ft_getcreate}, {LIST, &ft_getlist}, {GMSG, &ft_getgmsg},
		{DELET, &ft_getdelet}, {MSG, &ft_getmsg}, {WHO, &ft_getwho},
		{NONE, NULL}
	};

	i = 0;
	while (tab[i].type != NONE)
	{
		if (tab[i].type == sms->header.mytype)
		{
			tab[i].ccmds(sms, cli);
			return ;
		}
		i++;
	}
}

void	resive_srv(t_cli *cli)
{
	int		r;
	size_t	offset;
	t_sms	sms;

	offset = 0;
	ft_bzero(sms.sms, NAME_SIZE + 1);
	while ((r = recv(cli->fds[1], ((void *)&sms) + offset, BUF_SIZE, 0)) > 0)
	{
		offset += BUF_SIZE;
		if (offset == sizeof(t_sms))
		{
			handle_command(cli, &sms);
			return ;
		}
	}
	close(cli->fds[1]);
	ft_putendl("Server Error");
	exit(2);
}

void	check_fdc(t_cli *cli)
{
	while (cli->r > 0)
	{
		if (FD_ISSET(cli->fds[0], &cli->fd_read))
			read_std_e(cli);
		if (cli->info.connected && FD_ISSET(cli->fds[1], &cli->fd_read))
			resive_srv(cli);
		if (FD_ISSET(cli->fds[0], &cli->fd_read) || (cli->info.connected &&
		FD_ISSET(cli->fds[1], &cli->fd_read)))
			cli->r--;
	}
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
	cli->info.chan = 0;
	ft_strncpy(cli->info.name, "NONE", 4);
	cli->info.name[4] = '\0';
}

int		main(int ac, char **av)
{
	t_cli	cli;

	init_client(&cli);
	if (ac == 3)
		cli.fds[1] = connect_client(av[1], ft_atoi(av[2]), &cli);
	cli_loop(&cli);
	return (0);
}
