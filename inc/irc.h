/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 08:03:15 by amerelo           #+#    #+#             */
/*   Updated: 2018/01/17 05:24:42 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_H
# define IRC_H

# include "libft.h"
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>

# define FD_FREE			0
# define FD_SERV			1
# define FD_CLIENT			2
# define MAX_FD_CLI			2
# define NAME_SIZE			9
# define ROOM_NAME_SIZE		12
# define BUF_E_READ			1024
# define BUF_T				1024
# define BUF_SUB			2
# define SMS_PACKET_SIZE	512

# if BUF_SUB > SMS_PACKET_SIZE
#  error "Buffer subdivision is greater that the packet size !"
# endif

# if BUF_SUB < 0
#  error "Buffer subdivision is negative !"
# endif

# define SMS_MOD (SMS_PACKET_SIZE % BUF_SUB)

# if SMS_MOD != 0
#  error "Buffer subdivision size is not a multiple of packet size !"
# endif

enum	e_types
{
	NONE,
	CONNECT,
	NICK,
	JOIN,
	LEAVE,
	CREATE,
	DELET,
	LIST,
	WHO,
	MSG,
	GMSG
};

typedef struct		s_sms_header
{
	enum e_types	mytype;
	char			user[NAME_SIZE + 1];
}					t_sms_header;

# define SMS_SIZE	SMS_PACKET_SIZE - sizeof(t_sms_header)

typedef struct		s_sms
{
	t_sms_header	header;
	char			sms[SMS_SIZE];
}					t_sms;

# define BUF_SIZE	(sizeof(t_sms) / BUF_SUB)

typedef struct		s_msg
{
	int				global;
	int				read;
	int				write;
	char			user[NAME_SIZE + 1];
	char			buf_t[1024];
}					t_msg;

typedef struct		s_fd
{
	int				type;
	int				client;
	void			(*fct_read)();
	char			buf_read[BUF_SIZE + 1];
	char			name[NAME_SIZE + 1];
	t_msg			msg;
	struct s_chan	*chan;
}					t_fd;

typedef struct		s_info
{
	int				connected;
	int				chan;
	char			name[NAME_SIZE + 1];
}					t_info;

typedef struct		s_cli
{
	int				port;
	int				max;
	int				r;
	int				type1;
	int				type2;
	fd_set			fd_read;
	int				fds[MAX_FD_CLI];
	char			buf[BUF_SIZE + 1];
	t_info			info;
}					t_cli;

typedef struct		s_chan
{
	int				connected;
	char			name[ROOM_NAME_SIZE + 1];
	struct s_chan	*next;
}					t_chan;

typedef struct		s_env
{
	t_fd			*fds;
	t_chan			*chan;
	int				port;
	int				maxfd;
	int				max;
	int				r;
	fd_set			fd_read;
}					t_env;

typedef struct		s_cmd
{
	char			c[ROOM_NAME_SIZE + 1];
	void			(*cmds)(char*, t_cli *);
}					t_cmd;

typedef struct		s_ccmd
{
	enum e_types	type;
	void			(*ccmds)(t_sms *, t_cli *);
}					t_ccmd;

typedef struct		s_smd
{
	enum e_types	type;
	void			(*cmds)(char*, t_env *, int);
}					t_smd;

void				srv_loop(t_env *e);
void				send_packet(int cs, t_sms *sms);
void				send_to_client(char *txt, int cs, enum e_types type,
	char *name);
void				system_gmsg(t_env *e, int cs, char *sms);
void				send_msgs(t_env *e);
void				send_gmsg(t_env *e, int cs);
void				send_msg(t_env *e, int i);
void				send_packet(int cs, t_sms *sms);
void				srv_create(t_env *e, int port);
void				system_gmsg(t_env *e, int cs, char *sms);
void				srv_accept(t_env *e, int s);
void				read_client(t_env *e, int cs);
void				write_in_buf(t_env *e, int cs, char **tab);

void				handle_command(t_env *e, t_sms *sms, int cs);
void				ft_getnick(char *sms, t_env *e, int cs);
void				ft_getjoin(char *sms, t_env *e, int cs);
void				ft_getleave(char *sms, t_env *e, int cs);
void				ft_getdelet(char *sms, t_env *e, int cs);
void				ft_getcreate(char *sms, t_env *e, int cs);
void				ft_getlist(char *sms, t_env *e, int cs);

void				init_env(t_env *e);
void				init_list_chan(t_chan **head, char *name);
void				init_fd(t_env *e);
void				check_fd(t_env *e);
void				start(int port);
void				clean_fd(t_fd *fd);
void				usage(char *str);
int					get_client_by_name(t_env *e, char *name);
void				format_msg(t_env *e, char *sms, int cs);

void				init_fdc(t_cli *cli);
void				init_client(t_cli *cli);
int					connect_client(char *addr, int port, t_cli *cli);
void				chandle_command(t_cli *cli, t_sms *sms);
void				send_to_serv(char *txt, t_cli *cli, enum e_types type);
void				execut_command(char *cmd, t_cli *cli);

void				ft_cgetdelet(t_sms *sms, t_cli *cli);
void				ft_cgetlist(t_sms *sms, t_cli *cli);
void				ft_cgetwho(t_sms *sms, t_cli *cli);
void				ft_cgetgmsg(t_sms *sms, t_cli *cli);
void				ft_cgetmsg(t_sms *sms, t_cli *cli);
void				ft_cgetcreate(t_sms *sms, t_cli *cli);
void				ft_cgetleave(t_sms *sms, t_cli *cli);
void				ft_cgetjoin(t_sms *sms, t_cli *cli);
void				ft_cgetnick(t_sms *sms, t_cli *cli);

void				ft_sendconnect(char *buf, t_cli *cli);
void				ft_sendnick(char *buf, t_cli *cli);
void				ft_sendjoin(char *buf, t_cli *cli);
void				ft_sendleave(char *buf, t_cli *cli);
void				ft_sendcreate(char *buf, t_cli *cli);
void				ft_senddelet(char *buf, t_cli *cli);

void				cli_loop(t_cli *cli);
void				cclean_fd(t_cli *cli);
void				free_tab(char **tab);
int					starter_check(t_cli *cli);
int					verif_narg(char **tab, int limit);

#endif
