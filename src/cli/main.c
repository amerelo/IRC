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

void usage(char *str)
{
	ft_putstr(str);
	ft_putendl(" client");
	exit(2);
}

int create_client(char *addr, int port)
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

int main(int ac, char **av)
{
	int				port;
	int				sock;
	// int				cs;
	// unsigned int	sclen;
	// struct sockaddr	scin;
	// int				r;
	// char			buf[BUF_SIZE + 1];

	if (ac != 3)
		usage("need 3 args");

	port = ft_atoi(av[2]);
	sock = create_client(av[1], port);
	// cs = accept(sock, (struct sockaddr*)&csin, &cslen);
	// while ((r = read(cs, buf, BUF_SIZE) > 0))
	// {
	// 	buf[r] = '\0';
	// 	ft_putstr(buf);
	// }
	// close(cs);
	write(sock, "toto\0", 5);

	close(sock);
	return(0);
}
