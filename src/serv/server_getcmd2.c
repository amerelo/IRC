/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_getcmd2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:59:40 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:59:59 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	ft_getnick(char *sms, t_env *e, int cs)
{
	int		i;
	char	*tmp;

	i = 0;
	while (i < e->maxfd)
	{
		if (cs != i && e->fds[i].client == 1 &&
			ft_strcmp(e->fds[i].name, sms) == 0)
		{
			send_to_client("ERROR", cs, NICK, NULL);
			return ;
		}
		i++;
	}
	if (e->fds[cs].chan)
	{
		tmp = ft_strjoin(sms, "<- NEW NICKNAME\n");
		system_gmsg(e, cs, tmp);
		free(tmp);
	}
	ft_strcpy(e->fds[cs].name, sms);
	send_to_client(sms, cs, NICK, NULL);
}

void	ft_getjoin(char *sms, t_env *e, int cs)
{
	t_chan	*tmp;

	tmp = e->chan;
	while (tmp)
	{
		if (ft_strcmp(tmp->name, sms) == 0)
		{
			send_to_client(sms, cs, JOIN, NULL);
			e->fds[cs].chan = tmp;
			e->fds[cs].chan->connected++;
			system_gmsg(e, cs, " JOIN CHAN\n");
			return ;
		}
		tmp = tmp->next;
	}
	send_to_client("ERROR", cs, JOIN, NULL);
}

void	ft_getleave(char *sms, t_env *e, int cs)
{
	if (e->fds[cs].chan)
		system_gmsg(e, cs, " LEAVE CHAN\n");
	e->fds[cs].chan->connected--;
	e->fds[cs].chan = NULL;
	send_to_client(sms, cs, LEAVE, NULL);
}

void	ft_getdelet(char *sms, t_env *e, int cs)
{
	t_chan *tmp;
	t_chan *last;

	tmp = e->chan;
	last = NULL;
	while (tmp)
	{
		if (ft_strcmp(tmp->name, sms) == 0 && tmp->connected == 0)
		{
			if (last)
				last->next = tmp->next;
			else
				e->chan = tmp->next;
			free(tmp);
			send_to_client(sms, cs, DELET, NULL);
			return ;
		}
		last = tmp;
		tmp = tmp->next;
	}
	send_to_client("ERROR", cs, DELET, NULL);
}

void	ft_getcreate(char *sms, t_env *e, int cs)
{
	t_chan *tmp;

	tmp = e->chan;
	while (tmp)
	{
		if (ft_strcmp(tmp->name, sms) == 0)
		{
			send_to_client("ERROR", cs, CREATE, NULL);
			return ;
		}
		tmp = tmp->next;
	}
	init_list_chan(&e->chan, sms);
	send_to_client(sms, cs, CREATE, NULL);
}
