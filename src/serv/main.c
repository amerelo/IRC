/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 06:34:59 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:14:10 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	usage(char *str)
{
	ft_putendl(str);
	exit(2);
}

void	check_fd(t_env *e)
{
	int	i;

	i = 0;
	while ((i < e->maxfd) && (e->r > 0))
	{
		if (FD_ISSET(i, &e->fd_read))
			e->fds[i].fct_read(e, i);
		if (FD_ISSET(i, &e->fd_read))
			e->r--;
		i++;
	}
	send_msgs(e);
}

void	srv_loop(t_env *e)
{
	while (1)
	{
		init_fd(e);
		e->r = select(e->max + 1, &e->fd_read, NULL, NULL, NULL);
		check_fd(e);
	}
}

void	read_client(t_env *e, int cs)
{
	int		r;
	size_t	offset;
	t_sms	sms;

	if (e->fds[cs].type != FD_CLIENT)
		return ;
	offset = 0;
	while ((r = recv(cs, ((void *)&sms) + offset, BUF_SIZE, 0)) > 0)
	{
		offset += BUF_SIZE;
		if (offset == sizeof(t_sms))
		{
			handle_command(e, &sms, cs);
			return ;
		}
	}
	e->fds[cs].client = 0;
	if (e->fds[cs].chan)
		e->fds[cs].chan->connected -= 1;
	close(cs);
	clean_fd(&e->fds[cs]);
	ft_putendl("client gone away");
}

int		main(int ac, char **av)
{
	if (ac != 2)
		usage("need soket");
	start(ft_atoi(av[1]));
	return (0);
}
