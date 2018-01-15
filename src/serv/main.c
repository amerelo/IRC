/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 06:34:59 by amerelo           #+#    #+#             */
/*   Updated: 2017/12/11 08:06:04 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "irc.h"

void	usage(char *str)
{
	ft_putendl(str);
	exit(2);
}

void	clean_fd(t_fd *fd)
{
	fd->type = FD_FREE;
	fd->fct_read = NULL;
}

// ft_free_list_chan

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

void	srv_loop(t_env *e)
{
	while (1)
	{
		init_fd(e);
		e->r = select(e->max + 1, &e->fd_read, NULL, NULL, NULL);
		check_fd(e);
	}
}

void	start(int port)
{
	t_env	e;

	init_env(&e);
	srv_create(&e, port);
	srv_loop(&e);
}

void	init_fd(t_env *e)
{
	int	i;

	i = 0;
	e->max = 0;
	FD_ZERO(&e->fd_read);
	// FD_ZERO(&e->fd_write);
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

void	ft_getnick(char *sms, t_env *e, int cs)
{
	int i;

	i = 0;
	// printf("Get nick -> %s\n", sms);
	while (i < e->maxfd)
	{
		// if (e->fds[i].client == 1)
		// 	printf("name %s\n", e->fds[i].name);
		if (cs != i && e->fds[i].client == 1 &&
			ft_strcmp(e->fds[i].name, "NONE") != 0 &&
			ft_strcmp(e->fds[i].name, sms) == 0)
		{
			// printf("error name \n");
			send_to_client("ERROR", cs, NICK);
			return ;
		}
		i++;
	}
	ft_strcpy(e->fds[cs].name, sms);
	send_to_client(sms, cs, NICK);
}

void	ft_getjoin(char *sms, t_env *e, int cs)
{
	t_chan *tmp;

	tmp = e->chan;
	while (tmp)
	{
		if (ft_strcmp(tmp->name, sms) == 0)
		{
			e->fds[cs].chan = tmp;
			send_to_client(sms, cs, JOIN);
			return ;
		}
		tmp = tmp->next;
	}
	send_to_client("ERROR", cs, JOIN);
}

void	ft_getleave(char *sms, t_env *e, int cs)
{
	e->fds[cs].chan = NULL;
	send_to_client(sms, cs, LEAVE);
}

void	ft_getcreate(char *sms, t_env *e, int cs)
{
	t_chan *tmp;

	tmp = e->chan;
	while (tmp)
	{
		// printf("name %s\n", tmp->name);
		if (ft_strcmp(tmp->name, sms) == 0)
		{
			send_to_client("ERROR", cs, CREATE);
			return ;
		}
		tmp = tmp->next;
	}
	init_list_chan(&e->chan, sms);
	send_to_client(sms, cs, CREATE);
}

void	ft_getlist(char *sms, t_env *e, int cs)
{
	t_chan *tmp;

	(void)sms;
	tmp = e->chan;
	while (tmp)
	{
		send_to_client(tmp->name, cs, LIST);
		tmp = tmp->next;
	}
}

void	ft_getmsg(char *sms, t_env *e, int cs)
{
	int i;

	i = 0;
	while (sms[i])
	{
		if (e->fds[cs].msg.write == BUF_T)
			e->fds[cs].msg.write = 0;
		e->fds[cs].msg.buf_t[e->fds[cs].msg.write] = sms[i];
		e->fds[cs].msg.write++;
		i++;
	}
}

void	ft_getgmsg(char *sms, t_env *e, int cs)
{
	int i;

	i = 0;
	e->fds[cs].msg.global = 1;
	// check user
	// while (sms[i] == ' ')
	// {
	// 	user[NAME_SIZE + 1];
	// 	i++;
	//
	printf("get --------- %s\n", sms);
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
	int i;
	static t_smd tab[] = {
		{NICK, &ft_getnick}, {JOIN, &ft_getjoin}, {LEAVE, &ft_getleave},
		{CREATE, &ft_getcreate}, {LIST, &ft_getlist},
		{MSG, &ft_getmsg}, {GMSG, &ft_getgmsg},
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

void	send_packet(int cs, t_sms *sms)
{
	size_t	start;
	unsigned char *send_addr;

	start = 0;
	while (start < sizeof(*sms))
	{
		// printf("start: %lu - %lu\n", start, sizeof(*sms));
		send_addr = ((unsigned char *)sms) + start;
		send(cs, send_addr, BUF_SIZE, 0);
		start += BUF_SIZE;
	}
}

void	send_to_client(char* txt, int cs, enum types type)
{
	size_t	offset;
	char	buff[SMS_SIZE + 1];
	t_sms	sms;

	sms.header.mytype = type;
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

void	read_client(t_env *e, int cs)
{
	int		r;
	size_t	offset;
	t_sms	sms;

	if (e->fds[cs].type != FD_CLIENT)
		return ;
	offset = 0;
	while ((r = recv(cs, ((void *)&sms) + offset, BUF_SIZE, 0)) > 0)
	{
		offset += BUF_SIZE;
		if (offset == sizeof(t_sms))
		{
			handle_command(e, &sms, cs);
			return ;
		}
	}
	close(cs);
	clean_fd(&e->fds[cs]);
	e->fds[cs].client = 0;
	printf("client #%d gone away\n", cs);
}

void	srv_accept(t_env *e, int s)
{
	int					cs;
	struct sockaddr_in	csin;
	socklen_t			csin_len;

	csin_len = sizeof(csin);
	if ((cs = accept(s, (struct sockaddr*)&csin, &csin_len)) == -1)
		ft_putendl("accept error");
	clean_fd(&e->fds[cs]);
	e->fds[cs].type = FD_CLIENT;
	e->fds[cs].fct_read = read_client;
	ft_strncpy(e->fds[cs].name, "NONE", 4);
	e->fds[cs].name[4] = '\0';
	e->fds[cs].chan = NULL;
	e->fds[cs].client = 1;

	ft_putstr("New client ");
	ft_putendl(e->fds[cs].name);
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
		// printf("L|%c|\n", );
		sms[i] = e->fds[cs].msg.buf_t[e->fds[cs].msg.read];
		e->fds[cs].msg.read++;
		i++;
	}
	printf("F|%s|\n", sms);
	// sms[i] = '\0';
}

void send_msg(t_env *e, int i)
{
	int		client;
	char	sms[SMS_SIZE];

	printf("send_msg\n");
	ft_bzero(sms, SMS_SIZE);
	if ((client = get_client_by_name(e, e->fds[i].msg.user)) >= 0)
	{
		format_msg(e, sms, i);
		send_to_client(sms, client, MSG);
	}
	else
		send_to_client("ERROR", i, MSG);
	e->fds[i].msg.read++;
}

void send_gmsg(t_env *e, int cs)
{
	int		i;
	char	sms[SMS_SIZE + 1];

	i = 0;
	ft_bzero(sms, SMS_SIZE);
	format_msg(e, sms, cs);
	printf("format-----> %s\n", sms);
	while (i < e->maxfd)
	{
		if (i != cs)
			send_to_client(sms, i, GMSG);
		i++;
	}
}

void 	send_msgs(t_env *e)
{
	int		i;

	i = 0;
	printf("send\n");
	while (i < e->maxfd)
	{
		// printf("%s\n", );
		if (e->fds[i].client == 1 && e->fds[i].msg.global == 1)
		{
			// if (e->fds[i].msg.write != e->fds[i].msg.read)
			printf("global %d - %d\n", e->fds[i].msg.write, e->fds[i].msg.read);
			send_gmsg(e, i);
		}
		else if (e->fds[i].client == 0 && e->fds[i].msg.global == 0 &&
			e->fds[i].msg.write != e->fds[i].msg.read)
			send_msg(e, i);
		i++;
	}
}

void	check_fd(t_env *e)
{
	int		i;

	i = 0;
	while ((i < e->maxfd) && (e->r > 0))
	{
		if (FD_ISSET(i, &e->fd_read))
		{
			// ft_putendl("\ncall of read");
			e->fds[i].fct_read(e, i);
		}
		if (FD_ISSET(i, &e->fd_read))
			e->r--;
		i++;
	}
	send_msgs(e);
}

void	srv_create(t_env *e, int port)
{
	int		server_socket;
	struct	sockaddr_in serv;

	if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
		usage("socket fail");

	ft_memset((char *) &serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	int option_true = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option_true,
		sizeof(option_true));
	if (bind(server_socket , (struct sockaddr*)&serv, sizeof(serv) ) == -1)
		usage("socket fail");
	if (listen(server_socket, 42) == -1)
		usage("listen fail");
	e->fds[server_socket].type = FD_SERV;
	e->fds[server_socket].fct_read = srv_accept;
}

int		main(int ac, char** av)
{
	if (ac != 2)
		usage("need soket");
	start(ft_atoi(av[1]));
	return(0);
}
