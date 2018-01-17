/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 05:01:55 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:05:18 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	init_env(t_env *e)
{
	int				i;
	struct rlimit	rlp;

	i = 0;
	if (getrlimit(RLIMIT_NOFILE, &rlp) == -1)
		ft_putendl("error of getrlimit");
	e->maxfd = rlp.rlim_cur;
	e->chan = NULL;
	if (!(e->fds = malloc(sizeof(*e->fds) * e->maxfd)))
		ft_putendl("malloc error");
	while (i < e->maxfd)
	{
		clean_fd(&e->fds[i]);
		e->fds[i].client = 0;
		e->fds[i].msg.global = 1;
		e->fds[i].msg.read = 0;
		e->fds[i].msg.write = 0;
		ft_strcpy(e->fds[i].msg.user, "NONE");
		i++;
	}
}

void	init_list_chan(t_chan **head, char *name)
{
	t_chan *inf;

	inf = *head;
	while (inf && inf->next)
		inf = inf->next;
	if (!(*head))
	{
		inf = (t_chan *)malloc(sizeof(t_chan));
		*head = inf;
		ft_strcpy(inf->name, name);
		inf->connected = 0;
		inf->next = NULL;
	}
	else
	{
		inf->next = (t_chan *)malloc(sizeof(t_chan));
		inf = inf->next;
		ft_strcpy(inf->name, name);
		inf->connected = 0;
		inf->next = NULL;
	}
}

void	init_fd(t_env *e)
{
	int	i;

	i = 0;
	e->max = 0;
	FD_ZERO(&e->fd_read);
	while (i < e->maxfd)
	{
		if (e->fds[i].type != FD_FREE)
		{
			ft_bzero(e->fds[i].buf_read, BUF_SIZE + 1);
			FD_SET(i, &e->fd_read);
			e->max = (e->max > i) ? e->max : i;
		}
		i++;
	}
}

void	start(int port)
{
	t_env	e;

	init_env(&e);
	srv_create(&e, port);
	srv_loop(&e);
}
