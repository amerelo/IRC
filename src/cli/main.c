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
	FD_SET(cli->fds[1], &cli->fd_read);

	cli->max = cli->fds[1];
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

void	read_std_e(t_cli *cli)
{
	int			r;
	static char	buf[BUF_E_READ +1];

	ft_bzero(buf, BUF_E_READ +1);
	while ((r = read(1, buf, BUF_E_READ)) > 0)
	{
		send_to_serv(buf, cli);
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
	{
		// ft_putendl("read from standar");
		read_std_e(cli);
	}
	if (FD_ISSET(cli->fds[1], &cli->fd_read))
	{
		resive_srv(cli);
	}

	if (FD_ISSET(cli->fds[0], &cli->fd_read) ||
	FD_ISSET(cli->fds[1], &cli->fd_read))
		cli->r--;
	// if (FD_ISSET(i, &cli->fd_write))
	// 	cli->fds[i].fct_write(e, i);
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

void	cli_loop(t_cli *cli)
{
	while (1)
	{
		if (cli)

		init_fdc(cli);
		cli->r = select(cli->max + 1, &cli->fd_read, NULL, NULL, NULL);
		check_fdc(cli);
	}
}

int		main(int ac, char **av)
{
	t_cli	cli;

	if (ac != 3)
		usage("need 3 args");

	cli.port = ft_atoi(av[2]);
	cli.fds[0] = 1;
	cli.fds[1] = create_client(av[1], cli.port);

	// cli.type1 = FD_SERV;
	// cli.type2 = FD_SERV;
	cli_loop(&cli);
	return(0);
}
