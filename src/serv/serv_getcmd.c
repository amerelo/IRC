/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv_getcmd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:57:45 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:17:37 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	ft_getwho(char *sms, t_env *e, int cs)
{
	int i;

	(void)sms;
	i = 0;
	if (!e->fds[cs].chan)
	{
		send_to_client("ERROR", cs, WHO, NULL);
		return ;
	}
	while (i < e->maxfd)
	{
		if (e->fds[i].client == 1 && e->fds[i].chan == e->fds[cs].chan)
			send_to_client(e->fds[i].name, cs, WHO, NULL);
		i++;
	}
}

void	ft_getlist(char *sms, t_env *e, int cs)
{
	t_chan *tmp;

	(void)sms;
	tmp = e->chan;
	while (tmp)
	{
		send_to_client(tmp->name, cs, LIST, NULL);
		tmp = tmp->next;
	}
}

void	ft_getmsg(char *sms, t_env *e, int cs)
{
	char	**tab;

	tab = ft_strsplit(sms, ' ');
	if (get_client_by_name(e, tab[1]) == -1)
	{
		send_to_client("ERROR", cs, WHO, NULL);
		free_tab(tab);
		return ;
	}
	ft_strcpy(e->fds[cs].msg.user, tab[1]);
	e->fds[cs].msg.global = 0;
	write_in_buf(e, cs, tab);
	free_tab(tab);
}

void	ft_getgmsg(char *sms, t_env *e, int cs)
{
	int i;

	i = 0;
	e->fds[cs].msg.global = 1;
	while (sms[i])
	{
		if (e->fds[cs].msg.write == BUF_T)
			e->fds[cs].msg.write = 0;
		e->fds[cs].msg.buf_t[e->fds[cs].msg.write] = sms[i];
		e->fds[cs].msg.write++;
		i++;
	}
}

void	handle_command(t_env *e, t_sms *sms, int cs)
{
	int				i;
	static t_smd	tab[] = {
		{NICK, &ft_getnick}, {JOIN, &ft_getjoin}, {LEAVE, &ft_getleave},
		{CREATE, &ft_getcreate}, {LIST, &ft_getlist}, {WHO, &ft_getwho},
		{DELET, &ft_getdelet}, {MSG, &ft_getmsg}, {GMSG, &ft_getgmsg},
		{NONE, NULL}
	};

	i = 0;
	while (tab[i].type != NONE)
	{
		if (tab[i].type == sms->header.mytype)
			tab[i].cmds(sms->sms, e, cs);
		i++;
	}
}
