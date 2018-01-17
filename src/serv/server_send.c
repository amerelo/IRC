/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_send.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 05:07:26 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:10:31 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	send_gmsg(t_env *e, int cs)
{
	int		i;
	char	sms[SMS_SIZE + 1];

	i = 0;
	ft_bzero(sms, SMS_SIZE);
	format_msg(e, sms, cs);
	if (!e->fds[cs].chan)
		return ;
	while (i < e->maxfd)
	{
		if (i != cs && e->fds[i].client == 1 && (e->fds[i].chan &&
		e->fds[cs].chan == e->fds[i].chan))
		{
			send_to_client(sms, i, GMSG, e->fds[cs].name);
		}
		i++;
	}
}

void	send_msgs(t_env *e)
{
	int	i;

	i = 0;
	while (i < e->maxfd)
	{
		if (e->fds[i].client == 1 && e->fds[i].msg.global == 1 &&
			e->fds[i].msg.write != e->fds[i].msg.read)
			send_gmsg(e, i);
		else if (e->fds[i].client == 1 && e->fds[i].msg.global == 0 &&
			e->fds[i].msg.write != e->fds[i].msg.read)
			send_msg(e, i);
		i++;
	}
}

void	send_msg(t_env *e, int i)
{
	int		client;
	char	sms[SMS_SIZE];

	ft_bzero(sms, SMS_SIZE);
	if ((client = get_client_by_name(e, e->fds[i].msg.user)) >= 0)
	{
		format_msg(e, sms, i);
		send_to_client(sms, client, MSG, e->fds[i].name);
	}
	else
		send_to_client("ERROR", i, MSG, NULL);
}

void	send_packet(int cs, t_sms *sms)
{
	size_t			start;
	unsigned char	*send_addr;

	start = 0;
	while (start < sizeof(*sms))
	{
		send_addr = ((unsigned char *)sms) + start;
		send(cs, send_addr, BUF_SIZE, 0);
		start += BUF_SIZE;
	}
}

void	send_to_client(char *txt, int cs, enum e_types type, char *name)
{
	size_t	offset;
	char	buff[SMS_SIZE + 1];
	t_sms	sms;

	sms.header.mytype = type;
	if (name)
		ft_strcpy(sms.header.user, name);
	offset = 0;
	while (ft_strlen(txt) >= offset)
	{
		ft_strncpy(buff, txt + offset, SMS_SIZE);
		buff[SMS_SIZE] = 0;
		ft_memcpy(sms.sms, buff, SMS_SIZE + 1);
		send_packet(cs, &sms);
		offset += SMS_SIZE;
	}
}
