/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_send.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:33:27 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:34:09 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	ft_sendlist(char *buf, t_cli *cli)
{
	if (starter_check(cli))
		return ;
	send_to_serv(buf, cli, LIST);
}

void	ft_sendwho(char *buf, t_cli *cli)
{
	if (starter_check(cli) || cli->info.connected == 0)
		return ;
	send_to_serv(buf, cli, WHO);
}

void	ft_sendmsg(char *buf, t_cli *cli)
{
	char	**tab;

	if (ft_strlen(buf) <= 1)
		return ;
	tab = ft_strsplit(buf, ' ');
	if (verif_narg(tab, 3) < 0 || (ft_strlen(tab[1]) > NAME_SIZE) ||
		ft_strcmp(tab[1], "NONE") == 0)
	{
		ft_putstr("MSG need [user-name]-");
		ft_putnbr(ROOM_NAME_SIZE);
		ft_putendl("-maxsize [msg]");
		free_tab(tab);
		return ;
	}
	send_to_serv(buf, cli, MSG);
	free_tab(tab);
}

void	ft_sendgmsg(char *buf, t_cli *cli)
{
	if (cli->info.connected && ft_strlen(buf) > 1)
		send_to_serv(buf, cli, GMSG);
}

void	execut_command(char *cmd, t_cli *cli)
{
	int				i;
	static t_cmd	tab[] = {
		{"/connect", &ft_sendconnect}, {"/nick", &ft_sendnick},
		{"/join", &ft_sendjoin}, {"/leave", &ft_sendleave},
		{"/create", &ft_sendcreate}, {"/list", &ft_sendlist},
		{"/who", &ft_sendwho}, {"/msg", &ft_sendmsg},
		{"/delet", &ft_senddelet}, {"NONE", NULL}
	};

	i = 0;
	while (ft_strncmp(tab[i].c, "NONE", 4) != 0)
	{
		if (ft_strncmp(cmd, tab[i].c, ft_strlen(tab[i].c)) == 0)
		{
			tab[i].cmds(cmd, cli);
			return ;
		}
		i++;
	}
	if (cmd[0] == '/')
		ft_putendl("command not fund");
	else
		ft_sendgmsg(cmd, cli);
}
