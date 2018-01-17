/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_tools.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/17 04:38:34 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 04:45:00 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	cclean_fd(t_cli *cli)
{
	cli->type2 = FD_FREE;
	cli->info.connected = 0;
	cli->info.chan = 0;
	ft_bzero(cli->info.name, NAME_SIZE + 1);
	ft_strcpy(cli->info.name, "NONE");
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

int		verif_narg(char **tab, int limit)
{
	int i;

	i = 0;
	while (tab[i])
	{
		if (i >= limit)
			return (1);
		i++;
	}
	return (i - limit);
}
