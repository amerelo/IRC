/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_tools.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 05:04:21 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:05:52 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	clean_fd(t_fd *fd)
{
	fd->type = FD_FREE;
	fd->fct_read = NULL;
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

int		get_client_by_name(t_env *e, char *name)
{
	int i;

	i = 0;
	while (i < e->maxfd)
	{
		if (ft_strcmp(e->fds[i].name, name) == 0)
			return (i);
		i++;
	}
	return (-1);
}

void	format_msg(t_env *e, char *sms, int cs)
{
	int i;

	i = 0;
	while (e->fds[cs].msg.write != e->fds[cs].msg.read)
	{
		if (e->fds[cs].msg.read == BUF_T)
			e->fds[cs].msg.read = 0;
		sms[i] = e->fds[cs].msg.buf_t[e->fds[cs].msg.read];
		e->fds[cs].msg.read++;
		i++;
	}
}
