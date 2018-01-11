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

int		create_client(char *addr, int port)
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
		exit(2);
	}
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

void	send_to_serv(char* txt, t_cli *cli)
{
	size_t	offset;
	char	buff[SMS_SIZE + 1];
	t_sms	sms;

	sms.header.mytype = MSG;
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

void	ft_connect(char *buf, t_cli *cli)
{
	int		i;
	char	**server_ip;


	(void)cli;
	ft_strsplit(buf, ' ');
	while()
	{

	}
	printf("%s = %s\n", "ft_connect", buf);
	ft_strchr(buf, ' ');
	ft_strchr();
	// connect_to_server(cli, int port, char *ip);
}

void	ft_nick(char *buf, t_cli *cli)
{
	(void)cli;
	printf("%s = %s, \n", "ft_nick", buf);
}

void	ft_join(char *buf, t_cli *cli)
{
	(void)cli;
	printf("%s = %s, \n", "ft_join", buf);
}

void	ft_leave(char *buf, t_cli *cli)
{
	(void)cli;
	printf("%s = %s, \n", "ft_leave", buf);
}

void	ft_gmsg(char *buf, t_cli *cli)
{
	(void)cli;
	printf("%s = %s, \n", "ft_gmsg", buf);
}

int		execut_command(char *cmd, t_cli *cli)
{
	int i;
	// {"/gmsg", &ft_gmsg},
	static t_cmd tab[] = {
		{"/connect", &ft_connect}, {"/nick", &ft_nick},
		{"/join", &ft_join}, {"/leave", &ft_leave},
		{"NONE", NULL}
	};

	i = 0;
	while (ft_strcmp(tab[i].c, "NONE") != 0)
	{
		if (ft_strncmp(cmd, tab[i].c, ft_strlen(tab[i].c)) == 0)
		{
			tab[i].cmds(cmd, cli);
			return (1);
		}
		i++;
	}
	return (0);
}

void	pars_command(char *buf, t_cli *cli)
{
	// int command;

	execut_command(buf, cli);
	// if (*buf != '/' || (command = get_command(buf)) == 0)
	// {
	//    ft_putendl("ERROR command found !");
	//    return ;
	// }
}

void	read_std_e(t_cli *cli)
{
	int			r;
	static char	buf[BUF_E_READ +1];

	ft_bzero(buf, BUF_E_READ +1);
	while ((r = read(1, buf, BUF_E_READ)) > 0)
	{
		pars_command(buf, cli);
		// send_to_serv(buf, cli);
		if (ft_strchr(buf, '\n'))
			return ;
		// ft_bzero(buf, BUF_E_READ +1);
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
		// if (cli)
		init_fdc(cli);
		cli->r = select(cli->max + 1, &cli->fd_read, NULL, NULL, NULL);
		check_fdc(cli);
	}
}

void	connect_to_server(t_cli *cli, int port, char *ip)
{
	cli->port = port; // av[2]
	cli->fds[1] = create_client(ip, cli->port); // av[1]
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
