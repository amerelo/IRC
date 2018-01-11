/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amerelo <amerelo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/11 08:03:15 by amerelo           #+#    #+#             */
/*   Updated: 2017/12/11 08:04:55 by amerelo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_H
# define IRC_H

# include "libft.h"
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>

#include <stdio.h> //<< rm

# define FD_FREE			0
# define FD_SERV			1
# define FD_CLIENT			2
# define MAX_FD_CLI			2
# define NAME_SIZE			9
# define ROOM_NAME_SIZE		12

# define BUF_E_READ			1024
# define BUF_SUB			2
# define SMS_PACKET_SIZE	512

# if BUF_SUB > SMS_PACKET_SIZE
#  error "Buffer subdivision is greater that the packet size !"
# endif

# if BUF_SUB < 0
#  error "Buffer subdivision is negative !"
# endif

#define SMS_MOD (SMS_PACKET_SIZE % BUF_SUB)

# if SMS_MOD != 0
#  error "Buffer subdivision size is not a multiple of packet size !"
# endif

enum types
{
	CONNECT,
	NICK,
	JOIN,
	LEAVE,
	WHO,
	MSG,
	GMSG
};


typedef struct		s_sms_header
{
	enum types		mytype;
}					t_sms_header;

# define SMS_SIZE	SMS_PACKET_SIZE - sizeof(t_sms_header)

typedef struct		s_sms
{
	t_sms_header	header;
	char			sms[SMS_SIZE];
}					t_sms;

# define BUF_SIZE	(sizeof(t_sms) / BUF_SUB)

typedef struct		s_fd
{
	int				type;
	int				room;
	int				sizeread;
	void			(*fct_read)();
	char			buf_read[BUF_SIZE + 1];
	char			name[NAME_SIZE + 1];
	char			*struct_sms;
}					t_fd;

typedef struct		s_info
{
	int				connected;
	char			chan[ROOM_NAME_SIZE + 1];
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
	char			buf[BUF_SIZE +1];
	t_info			info;
}					t_cli;

typedef struct		s_env
{
	t_fd			*fds;
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
} 					t_cmd;

void				srv_create(t_env *e, int port);

void				check_fd(t_env *e);
void				init_fd(t_env *e);
// void	clean_fd(t_fd *fd);

#endif
