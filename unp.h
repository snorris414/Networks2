/* include unp.h */
#ifndef __unp_h
#define __unp_h
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* basic system data types */
/* basic socket definitions */
/* sockaddr_in{} and other Internet defns */
#include <sys/time.h> /* timeval{} for select() */
#include <time.h> /* timespec{} for pselect() */
#include <netdb.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#define SERV_PORT 10010 /* TCP and UDP */
#define MAXLINE 4096 /* max text line length */
#define BUFFSIZE 8192 /* buffer size for reads and writes */

#endif /* __unp_h */
