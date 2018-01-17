/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:24:22 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:25:41 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	init_client(t_cli *cli)
{
	cli->fds[0] = 1;
	cli->fds[1] = 0;
	cli->info.connected = 0;
	cli->info.chan = 0;
	ft_strncpy(cli->info.name, "NONE", 4);
	cli->info.name[4] = '\0';
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
