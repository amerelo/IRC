/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_get.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:27:12 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:48:46 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	ft_cgetlist(t_sms *sms, t_cli *cli)
{
	(void)cli;
	ft_putstr("->Room: ");
	ft_putendl(sms->sms);
}

void	ft_cgetwho(t_sms *sms, t_cli *cli)
{
	(void)cli;
	ft_putstr("->User: ");
	ft_putendl(sms->sms);
}

void	ft_cgetgmsg(t_sms *sms, t_cli *cli)
{
	static int	flag = -1;

	(void)cli;
	if (sms->sms[0] == '\0')
		return ;
	if (flag == -1 && ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else if (flag == -1)
		flag = 0;
	if (flag == 1)
	{
		ft_putstr(sms->header.user);
		ft_putstr(": ");
	}
	if (ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else
		flag = 0;
	ft_putstr(sms->sms);
}

void	ft_cgetmsg(t_sms *sms, t_cli *cli)
{
	static int	flag = -1;

	(void)cli;
	if (sms->sms[0] == '\0')
		return ;
	if (flag == -1 && ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else if (flag == -1)
		flag = 0;
	if (flag == 1)
	{
		ft_putstr("(PRIVATE)");
		ft_putstr(sms->header.user);
		ft_putstr(": ");
	}
	if (ft_strrchr(sms->sms, '\n') != NULL)
		flag = 1;
	else
		flag = 0;
	ft_putstr(sms->sms);
}

void	chandle_command(t_cli *cli, t_sms *sms)
{
	int				i;
	static t_ccmd	tab[] = {
		{NICK, &ft_cgetnick}, {JOIN, &ft_cgetjoin}, {LEAVE, &ft_cgetleave},
		{CREATE, &ft_cgetcreate}, {LIST, &ft_cgetlist}, {GMSG, &ft_cgetgmsg},
		{DELET, &ft_cgetdelet}, {MSG, &ft_cgetmsg}, {WHO, &ft_cgetwho},
		{NONE, NULL}
	};

	i = 0;
	while (tab[i].type != NONE)
	{
		if (tab[i].type == sms->header.mytype)
		{
			tab[i].ccmds(sms, cli);
			return ;
		}
		i++;
	}
}
