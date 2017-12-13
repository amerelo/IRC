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

void usage(char *str)
{
	ft_putendl(str);
	exit(2);
}

void	srv_create(char *port)
{
	int		server_socket;
	struct	sockaddr_in serv;
	fd_set	readfd;
	// struct protoent *proto;

	if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		usage("socket fail");

	ft_memset((char *) &serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(ft_atoi(port));
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(server_socket , (struct sockaddr*)&serv, sizeof(serv) ) == -1)
		usage("socket fail");
	listen(server_socket, 42);


	FD_ZERO(&(readfd));
	FD_SET(server_socket, &(readfd));

	int cs;
	unsigned int cslen;
	struct sockaddr_in csin;
	int r;
	char buf[1024];

	cs = accept(server_socket, (struct sockaddr*)&csin, &cslen);
	r = read(cs, buf, 1023);
	buf[r] = '\0';
	if (r > 0)
		ft_putendl(buf);

}

int		main(int ac, char** av)
{
	if (ac != 2)
		usage("need soket");
	srv_create(av[1]);
	return(0);
}
