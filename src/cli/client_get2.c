/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_get2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:28:57 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:46:14 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	ft_cgetnick(t_sms *sms, t_cli *cli)
{
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("Nick name not valid try again");
		return ;
	}
	ft_strcpy(cli->info.name, sms->sms);
	ft_putendl("Valid nick name");
}

void	ft_cgetjoin(t_sms *sms, t_cli *cli)
{
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("Chat Room not valid try again");
		return ;
	}
	cli->info.chan = 1;
	ft_putendl("Join room");
}

void	ft_cgetleave(t_sms *sms, t_cli *cli)
{
	(void)sms;
	cli->info.chan = 0;
	ft_putendl("Leave room");
}

void	ft_cgetcreate(t_sms *sms, t_cli *cli)
{
	(void)cli;
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("Create fail try again");
		return ;
	}
	ft_putstr("Create room ");
	ft_putendl(sms->sms);
}

void	ft_cgetdelet(t_sms *sms, t_cli *cli)
{
	(void)cli;
	if (ft_strcmp(sms->sms, "ERROR") == 0)
	{
		ft_putendl("DELET fail try again");
		return ;
	}
	ft_putstr("DELET room ");
	ft_putendl(sms->sms);
}
