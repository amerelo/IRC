/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_recive.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:31:29 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:48:53 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

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
			chandle_command(cli, &sms);
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
