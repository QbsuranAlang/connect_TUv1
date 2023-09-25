/*
 * MIT License
 * 
 * Copyright (c) 2023 connect_TUv1
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <net/if.h>
#include <spawn.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/sctp.h>

#include "dccp.h"
#include "connect_TUv1.h"

#define ANY_IPv4        "0.0.0.0"
#define ANY_IPv6        "::"
#define SERVER_IPv4     "127.0.0.1"
#define SERVER_IPv6     "::1"
#define LOOPBACK_IPv4   "127.0.0.1"
#define LOOPBACK_IPv6   "::1"

#define TEXT_TCP_IPv4    "Hello from TCPv4 client"
#define TEXT_TCP_IPv6    "Hello from TCPv6 client"
#define TEXT_UDP_IPv4    "Hello from UDPv4 client"
#define TEXT_UDP_IPv6    "Hello from UDPv6 client"
#define TEXT_SCTP_IPv4   "Hello from SCTPv4 client"
#define TEXT_SCTP_IPv6   "Hello from SCTPv6 client"
#define TEXT_DCCP_IPv4   "Hello from DCCPv4 client"
#define TEXT_DCCP_IPv6   "Hello from DCCPv6 client"

#define DCCP_SERVICE_CODE 9601

#define NO_EINTR(stmt) while((stmt) == -1 && errno == EINTR);

extern int          opterr;
extern sa_family_t  sa_family;
extern int          sock_type;
extern int          ip_proto;
extern const char   *ip_proto_str;
extern int          server_fd;
extern int          client_fd;
extern bool         use_old;
extern bool         save;
extern bool         stop;
extern in_port_t    client_port;
extern in_port_t    server_port;
extern pthread_t    tid;
extern bool         lock;

struct proto_ops {
    const char  *proto;
    const char  *send_message_v4;
    const char  *send_message_v6;

    /* server side */
    int         (*server_setup)(void);
    int         (*start_server)(const char *proto, void *(*main_server)(void *arg));
    void        *(*main_server)(void *arg);
    void        (*stop_server)(void);

    /* client side */
    int         (*client_jobs)(const char *send_message);
};

/* pcap.c */
pid_t start_tcpdump(const char *filename_prefix);
void stop_tcpdump(pid_t pid);

/* rand.c */
void init_rand(void);
in_port_t rand_a_port(void);

/* spawn.c */
int spawn_async(pid_t *child_pid, int *out_fno, int *err_fno, char *argv[], char *envp[]);
int spawn_sync(pid_t *child_pid, int *status, int *out_fno, int *err_fno, char *argv[], char *envp[]);

/* fileutils */
bool file_is_exist(const char *filename);

/* for server side */
int server_setup_common(struct sockaddr_storage *ss);
int server_bind_and_listen_common(struct sockaddr_storage *ss);
int server_bind_common(struct sockaddr_storage *ss);
int server_main_stream_server_accept4_and_echo_common(void);
int server_main_dgram_server_recvfrom_and_echo_common(void);

/* server side for main function */
int start_server(const char *proto, void *(*main_server)(void *));

/* for client side */
int client_setup_common(struct sockaddr_storage *ss);
int client_stream_connect_and_echo(struct sockaddr_storage *ss, const char *send_message);
int client_dgram_connect_and_echo(struct sockaddr_storage *ss, const char *send_message);

/* tcp_server.c */
int tcp_server_setup(void);
void stop_tcp_server(void);
void *tcp_main_server(void *arg);

/* tcp_client.c */
int tcp_client_jobs(const char *send_message);

/* udp_server.c */
int udp_server_setup(void);
void stop_udp_server(void);
void *udp_main_server(void *arg);

/* udp_client.c */
int udp_client_jobs(const char *send_message);

/* sctp_server.c */
int sctp_server_setup(void);
void stop_sctp_server(void);
void *sctp_main_server(void *arg);

/* sctp_client.c */
int sctp_client_jobs(const char *send_message);

/* dccp_server.c */
int dccp_server_setup(void);
void stop_dccp_server(void);
void *dccp_main_server(void *arg);

/* dccp_client.c */
int dccp_client_jobs(const char *send_message);

#endif /* COMMON_H */
