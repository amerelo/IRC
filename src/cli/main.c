/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 06:34:59 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:23:06 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	csend_packet(t_cli *cli, t_sms *sms)
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

void	send_to_serv(char *txt, t_cli *cli, enum e_types type)
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
		csend_packet(cli, &sms);
		offset += SMS_SIZE;
	}
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

int		main(int ac, char **av)
{
	t_cli	cli;

	init_client(&cli);
	if (ac == 3)
		cli.fds[1] = connect_client(av[1], ft_atoi(av[2]), &cli);
	cli_loop(&cli);
	return (0);
}
