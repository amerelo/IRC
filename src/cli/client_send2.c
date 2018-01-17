/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_send2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:35:24 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:52:57 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	ft_sendnick(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (!cli->info.connected)
	{
		ft_putendl("Need to connection to server");
		return ;
	}
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > NAME_SIZE) ||
		(ft_strcmp(tab[1], "NONE") == 0 || ft_strcmp(tab[1], "ERROR") == 0))
	{
		ft_putstr("Nick need [nick-name]-");
		ft_putnbr(NAME_SIZE);
		ft_putendl("-maxsize or nick not valid");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, NICK);
	free_tab(tab);
}

void	ft_sendjoin(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (starter_check(cli))
		return ;
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > NAME_SIZE))
	{
		ft_putstr("Join need [room-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, JOIN);
	free_tab(tab);
}

void	ft_sendleave(char *buf, t_cli *cli)
{
	if (!cli->info.connected)
	{
		ft_putendl("need to connection to server");
		return ;
	}
	send_to_serv(buf, cli, LEAVE);
}

void	ft_sendcreate(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (starter_check(cli))
		return ;
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > ROOM_NAME_SIZE))
	{
		ft_putstr("CREATE need [room-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, CREATE);
	free_tab(tab);
}

void	ft_senddelet(char *buf, t_cli *cli)
{
	char	**tab;
	char	*tmp;

	if (starter_check(cli))
		return ;
	if ((tmp = ft_strrchr(buf, '\n')) != NULL)
		*tmp = '\0';
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 2) != 0 || (ft_strlen(tab[1]) > ROOM_NAME_SIZE))
	{
		ft_putstr("DELET need [room-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize");
		free_tab(tab);
		return ;
	}
	send_to_serv(tab[1], cli, DELET);
	free_tab(tab);
}
