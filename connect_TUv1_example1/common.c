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


#include "common.h"

sa_family_t sa_family = PF_UNSPEC;
int         sock_type = SOCK_RAW;
int         ip_proto = IPPROTO_IP;
const char  *ip_proto_str = NULL;
int         server_fd = -1;
int         client_fd = -1;
bool        use_old = true;
bool        save = false;
bool        stop = false;
in_port_t   client_port = 0;
in_port_t   server_port = 0;
pthread_t   tid = -1;
bool        lock = false;

static int fill_sockaddr_storage(struct sockaddr_storage *ss, const char *ipv4_addr, const char *ipv6_addr) {
    int                 ret;
    struct sockaddr_in  *in;
    struct sockaddr_in6 *in6;

    memset(ss, 0, sizeof(struct sockaddr_storage));
    if(sa_family == PF_INET) {
        in = (struct sockaddr_in *)ss;
        in->sin_family = PF_INET;
        in->sin_port = htons(server_port);
        ret = inet_pton(PF_INET, ipv4_addr, &in->sin_addr);
    } else if(sa_family == PF_INET6) {
        in6 = (struct sockaddr_in6 *)ss;
        in6->sin6_family = PF_INET6;
        in6->sin6_port = htons(server_port);
        in6->sin6_scope_id = if_nametoindex("lo");
        ret = inet_pton(PF_INET6, ipv6_addr, &in6->sin6_addr);
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        return -1;
    }//end else

    if(ret == 0) {
        errno = EINVAL;
        fprintf(stderr, "inet_pton(): %s\n", strerror(errno));
        return -1;
    } else if(ret == -1) {
        fprintf(stderr, "inet_pton(): %s\n", strerror(errno));
        return -1;
    }//end if

    return 0;
}//end fill_sockaddr_storage

static int fd_set_timeout(int fd) {
    int             ret;
    struct timeval  tv;

    /* set timeout */
    tv.tv_sec = 1;
    tv.tv_usec = 500;

    ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(SO_RCVTIMEO): %s\n", strerror(errno));
        return -1;
    }//end if

    ret = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(SO_SNDTIMEO): %s\n", strerror(errno));
        return -1;
    }//end if

    return 0;
}//end fd_set_timeout

static int fd_set_reuseaddr_and_reuseport(int fd) {
    int ret, optval;

    /* set reuseaddr and reuseport */
    optval = 1;

    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(SO_REUSEADDR): %s\n", strerror(errno));
        return -1;
    }//end if

    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(SO_REUSEPORT): %s\n", strerror(errno));
        return -1;
    }//end if

    return 0;
}//end fd_set_reuseaddr_and_reuseport

static int fd_set_bind_and_listen(int fd, struct sockaddr_storage *ss) {
    int ret;

    if(sa_family == PF_INET) {
        ret = bind(fd, (struct sockaddr_in *)ss, sizeof(struct sockaddr_in));
    } else if(sa_family == PF_INET6) {
        ret = bind(fd, (struct sockaddr_in6 *)ss, sizeof(struct sockaddr_in6));
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        return -1;
    }//end else

    if(ret != 0) {
        fprintf(stderr, "bind(): %s\n", strerror(errno));
        return -1;
    }//end if

    ret = listen(fd, 5);
    if(ret != 0) {
        fprintf(stderr, "listen(): %s\n", strerror(errno));
        return -1;
    }//end if

    return 0;
}//end fd_set_bind_and_listen

int server_bind_common(struct sockaddr_storage *ss) {
    int ret;

    if(sa_family == PF_INET) {
        ret = bind(server_fd, (struct sockaddr_in *)ss, sizeof(struct sockaddr_in));
    } else if(sa_family == PF_INET6) {
        ret = bind(server_fd, (struct sockaddr_in6 *)ss, sizeof(struct sockaddr_in6));
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        return -1;
    }//end else

    if(ret != 0) {
        fprintf(stderr, "bind(): %s\n", strerror(errno));
        return -1;
    }//end if

    return 0;
}//end server_bind_common

int server_setup_common(struct sockaddr_storage *ss) {
    int ret;

    /* fill sockaddr_storage */
    ret = fill_sockaddr_storage(ss, SERVER_IPv4, SERVER_IPv6);
    if(ret != 0) {
        return -1;
    }//end if

    /* set timeout */
    ret = fd_set_timeout(server_fd);
    if(ret != 0) {
        return -1;
    }//end if

    /* set reuseaddr and reuseport */
    ret = fd_set_reuseaddr_and_reuseport(server_fd);
    if(ret != 0) {
        return -1;
    }//end if

    return 0;
}//end server_setup_common

int server_bind_and_listen_common(struct sockaddr_storage *ss) {
    int ret;

    ret = fd_set_bind_and_listen(server_fd, ss);
    if(ret != 0) {
        return -1;
    }//end if

    return 0;
}//end server_bind_and_listen_common

static int print_stream_info(void) {
    int                     ret;
    char                    buf[256], local_buf[256], peer_buf[256];
    socklen_t               addrlen;
    in_port_t               port;
    const char              *ptr;
    struct sockaddr_in      *in;
    struct sockaddr_in6     *in6;
    struct sockaddr_storage ss, local_ss, peer_ss;

    /* getsockname() and getpeername() */
    memset(&local_ss, 0, sizeof(struct sockaddr_storage));
    memset(&peer_ss, 0, sizeof(struct sockaddr_storage));

    /* local address and port */
    addrlen = sizeof(struct sockaddr_storage);
    ret = getsockname(client_fd, (struct sockaddr *)&ss, &addrlen);
    if(ret != 0) {
        fprintf(stderr, "getpeername(): %s\n", strerror(errno));
        return -1;
    }//end if

    memset(buf, 0, sizeof(buf));
    if(ss.ss_family == PF_INET) {
        in = (struct sockaddr_in *)&ss;
        ptr = inet_ntop(ss.ss_family, &in->sin_addr, buf, sizeof(buf));
        port = ntohs(in->sin_port);
    } else if(ss.ss_family == PF_INET6) {
        in6 = (struct sockaddr_in6 *)&ss;
        ptr = inet_ntop(ss.ss_family, &in6->sin6_addr, buf, sizeof(buf));
        port = ntohs(in6->sin6_port);
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        return -1;
    }//end else

    if(!ptr) {
        fprintf(stderr, "inet_ntop(): %s\n", strerror(errno));
        return -1;
    }//end if

    if(ss.ss_family == PF_INET) {
        snprintf(local_buf, sizeof(local_buf), "%s:%d", ptr, port);
    } else if(ss.ss_family == PF_INET6) {
        snprintf(local_buf, sizeof(local_buf), "[%s]:%d", ptr, port);
    }//end else

    /* peer address and port */
    ret = getpeername(client_fd, (struct sockaddr *)&ss, &addrlen);
    if(ret != 0) {
        fprintf(stderr, "getsockname(): %s\n", strerror(errno));
    }//end if

    memset(buf, 0, sizeof(buf));
    if(ss.ss_family == PF_INET) {
        in = (struct sockaddr_in *)&ss;
        ptr = inet_ntop(ss.ss_family, &in->sin_addr, buf, sizeof(buf));
        port = ntohs(in->sin_port);
    } else if(ss.ss_family == PF_INET6) {
        in6 = (struct sockaddr_in6 *)&ss;
        ptr = inet_ntop(ss.ss_family, &in6->sin6_addr, buf, sizeof(buf));
        port = ntohs(in6->sin6_port);
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        return -1;
    }//end else

    if(!ptr) {
        fprintf(stderr, "inet_ntop(): %s\n", strerror(errno));
        return -1;
    }//end if

    if(ss.ss_family == PF_INET) {
        snprintf(peer_buf, sizeof(peer_buf), "%s:%d", ptr, port);
    } else if(ss.ss_family == PF_INET6) {
        snprintf(peer_buf, sizeof(peer_buf), "[%s]:%d", ptr, port);
    }//end else

    printf("%s Connection: %s <-> %s\n", ip_proto_str, local_buf, peer_buf);
    return 0;
}//end print_stream_info

int server_main_stream_server_accept4_and_echo_common(void) {
    int                     client_fd, ret, s_recv, s_send, flags;
    char                    buf[256];
    ssize_t                 n_recv, n_send;
    socklen_t               addrlen;
    struct sctp_sndrcvinfo  sndrcvinfo;
    struct sockaddr_storage ss;

    /* accept4() new incoming client */
    memset(&ss, 0, sizeof(ss));
    addrlen = sizeof(struct sockaddr_storage);
    client_fd = accept4(server_fd, (struct sockaddr *)&ss, &addrlen, SOCK_CLOEXEC);
    if(client_fd < 0) {
        if(errno != EAGAIN) {
            fprintf(stderr, "accept4(): %s\n", strerror(errno));
        }//end if
        return -1;
    }//end if

    /* connection information */
    ret = print_stream_info();
    if(ret != 0) {
        return -1;
    }//end if

    lock = false;
    sleep(1);

    if(ip_proto == IPPROTO_TCP || ip_proto == IPPROTO_DCCP) {
        /* recv(), send() and echo */
        memset(buf, 0, sizeof(buf));
        n_recv = recv(client_fd, buf, sizeof(buf), 0);
        if(n_recv < 0) {
            fprintf(stderr, "recv(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if
        printf("server: recv(%zd bytes): %s, tid: %d\n", n_recv, buf, gettid());

        n_send = send(client_fd, buf, strlen(buf) + 1, 0);
        if(n_send < 0) {
            fprintf(stderr, "send(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if
        printf("server: send(%zd bytes): %s, tid: %d\n", n_send, buf, gettid());
    } else if(ip_proto == IPPROTO_SCTP) {
        /* sctp_recvmsg(), sctp_sendmsg() and echo */
        memset(buf, 0, sizeof(buf));
        s_recv = sctp_recvmsg(client_fd, buf, sizeof(buf), (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags);
        if(s_recv < 0) {
            fprintf(stderr, "sctp_recvmsg(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if
        printf("server: sctp_recvmsg(%d bytes): %s, tid: %d\n", s_recv, buf, gettid());

        s_send = sctp_sendmsg(client_fd, buf, strlen(buf) + 1, NULL, 0, 0, 0, 0, 0, 0);
        if(s_send < 0) {
            fprintf(stderr, "sctp_sendmsg(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if
        printf("server: sctp_sendmsg(%d bytes): %s, tid: %d\n", s_send, buf, gettid());
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        close(client_fd);
        return -1;
    }//end else

    close(client_fd);
    return 0;
}//end server_main_stream_server_accept4_and_echo_common

int print_dgram_info(struct sockaddr_storage *peer) {
    int                     ret;
    char                    buf[256], local_buf[256], peer_buf[256];
    in_port_t               port;
    socklen_t               addrlen;
    const char              *ptr;
    struct sockaddr_in      *in;
    struct sockaddr_in6     *in6;
    struct sockaddr_storage ss;

    memset(buf, 0, sizeof(buf));
    if(sa_family == PF_INET) {
        in = (struct sockaddr_in *)peer;
        ptr = inet_ntop(sa_family, &in->sin_addr, buf, sizeof(buf));
        port = ntohs(in->sin_port);
    } else if(sa_family == PF_INET6) {
        in6 = (struct sockaddr_in6 *)peer;
        ptr = inet_ntop(sa_family, &in6->sin6_addr, buf, sizeof(buf));
        port = ntohs(in6->sin6_port);
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        return -1;
    }//end else

    if(sa_family == PF_INET) {
        snprintf(local_buf, sizeof(local_buf), "%s:%d", ptr, port);
    } else if(sa_family == PF_INET6) {
        snprintf(local_buf, sizeof(local_buf), "[%s]:%d", ptr, port);
    }//end else

    /* local address and port */
    memset(&ss, 0, sizeof(ss));
    addrlen = sizeof(struct sockaddr_storage);
    ret = getsockname(server_fd, (struct sockaddr *)&ss, &addrlen);
    if(ret != 0) {
        fprintf(stderr, "getsockname(): %s\n", strerror(errno));
    }//end if

    memset(buf, 0, sizeof(buf));
    if(ss.ss_family == PF_INET) {
        in = (struct sockaddr_in *)&ss;
        ptr = inet_ntop(ss.ss_family, &in->sin_addr, buf, sizeof(buf));
        port = ntohs(in->sin_port);
    } else if(ss.ss_family == PF_INET6) {
        in6 = (struct sockaddr_in6 *)&ss;
        ptr = inet_ntop(ss.ss_family, &in6->sin6_addr, buf, sizeof(buf));
        port = ntohs(in6->sin6_port);
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        return -1;
    }//end else

    if(!ptr) {
        fprintf(stderr, "inet_ntop(): %s\n", strerror(errno));
        return -1;
    }//end if

    if(sa_family == PF_INET) {
        snprintf(peer_buf, sizeof(peer_buf), "%s:%d", ptr, port);
    } else if(ss.ss_family == PF_INET6) {
        snprintf(peer_buf, sizeof(peer_buf), "[%s]:%d", ptr, port);
    }//end else

    printf("%s Connection: %s <-> %s\n", ip_proto_str, local_buf, peer_buf);
    return 0;
}//end print_dgram_info

int server_main_dgram_server_recvfrom_and_echo_common(void) {
    int                     ret;
    char                    buf[256];
    ssize_t                 n_recv, n_send;
    socklen_t               addrlen;
    struct sockaddr_storage ss;

    /* accept4() new incoming client */
    memset(&ss, 0, sizeof(ss));
    addrlen = sizeof(struct sockaddr_storage);

    /* recvfrom(), sendto() and echo */
    memset(buf, 0, sizeof(buf));
    n_recv = recvfrom(server_fd, buf, sizeof(buf), 0, (struct sockaddr *)&ss, &addrlen);
    if(n_recv < 0) {
        fprintf(stderr, "recvfrom(): %s\n", strerror(errno));
        return -1;
    }//end if

    /* connection information */
    ret = print_dgram_info(&ss);
    if(ret != 0) {
        return -1;
    }//end if

    lock = false;
    sleep(1);

    printf("server: recvfrom(%zd bytes): %s, tid: %d\n", n_recv, buf, gettid());

    n_send = sendto(server_fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&ss, addrlen);
    if(n_send < 0) {
        fprintf(stderr, "sendto(): %s\n", strerror(errno));
        close(client_fd);
        return -1;
    }//end if
    printf("server: sendto(%zd bytes): %s, tid: %d\n", n_send, buf, gettid());

    return 0;
}//end server_main_dgram_server_recvfrom_and_echo_common

int start_server(const char *proto, void *(*main_server)(void *)) {
    int ret;

    printf("%s server is listening on %d\n", proto, server_port);

    tid = -1;
    stop = true;
    ret = pthread_create(&tid, 0, main_server, (void *)proto);
    if(ret != 0) {
        fprintf(stderr, "pthread_create(): %s\n", strerror(ret));
        return -1;
    }//end if

    /* stop loop is start */
    while(stop) {
        usleep(1000);
    }//end while

    return 0;
}//end start_server

int client_setup_common(struct sockaddr_storage *ss) {
    if(ip_proto == IPPROTO_SCTP) {
        return fill_sockaddr_storage(ss, LOOPBACK_IPv4, LOOPBACK_IPv6);
    } else {
        return fill_sockaddr_storage(ss, ANY_IPv4, ANY_IPv6);
    }//end else
}//end client_setup_common

static int client_connect(struct sockaddr_storage *ss) {
    int         ret;
    socklen_t   addrlen;

    /* call connect() or connect_TUv1() */
    addrlen = sizeof(struct sockaddr_storage);
    if(use_old) {
        ret = connect(client_fd, (struct sockaddr *)ss, addrlen);
    } else {
        ret = connect_TUv1(client_fd, (struct sockaddr *)ss, addrlen, client_port);
    }//end else

    if(ret != 0) {
        fprintf(stderr, "%s(): %s\n", use_old ? "connect" : "connect_TUv1", strerror(errno));
        return -1;
    }//end if

    return 0;
}//end client_connect

int client_stream_connect_and_echo(struct sockaddr_storage *ss, const char *send_message) {
    int                     ret, s_send, s_recv, flags;
    char                    buf[256];
    ssize_t                 n_recv, n_send;
    struct sctp_sndrcvinfo  sndrcvinfo;

    ret = client_connect(ss);
    if(ret != 0) {
        return -1;
    }//end if

    if(ip_proto == IPPROTO_TCP || ip_proto == IPPROTO_DCCP) {
        /* send(), recv() and echo */
        n_send = send(client_fd, send_message, strlen(send_message) + 1, 0);
        if(n_send < 0) {
            fprintf(stderr, "send(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if

        lock = true;
        while(lock) {
            usleep(1000 * 50);
        }//end while

        printf("client: send(%zd bytes): %s, tid: %d\n", n_send, send_message, gettid());

        memset(buf, 0, sizeof(buf));
        n_recv = recv(client_fd, buf, sizeof(buf), 0);
        if(n_recv < 0) {
            fprintf(stderr, "recv(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if
        printf("client: recv(%zd bytes): %s, tid: %d\n", n_recv, buf, gettid());
    } else if(ip_proto == IPPROTO_SCTP) {
        /* sctp_sendmsg(), sctp_recvmsg() and echo */
        s_send = sctp_sendmsg(client_fd, send_message, strlen(send_message) + 1, NULL, 0, 0, 0, 0, 0, 0);
        if(s_send < 0) {
            fprintf(stderr, "sctp_sendmsg(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if

        lock = true;
        while(lock) {
            usleep(1000 * 50);
        }//end while

        printf("client: sctp_sendmsg(%d bytes): %s, tid: %d\n", s_send, send_message, gettid());

        memset(buf, 0, sizeof(buf));
        s_recv = sctp_recvmsg(client_fd, buf, sizeof(buf), (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags);
        if(s_recv < 0) {
            fprintf(stderr, "sctp_recvmsg(): %s\n", strerror(errno));
            close(client_fd);
            return -1;
        }//end if
        printf("client: sctp_recvmsg(%d bytes): %s, tid: %d\n", s_recv, buf, gettid());
    } else {
        fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
        close(client_fd);
        return -1;
    }//end else

    return 0;
}//end client_stream_connect_and_echo

int client_dgram_connect_and_echo(struct sockaddr_storage *ss, const char *send_message) {
    int         ret;
    char        buf[256];
    ssize_t     n_recv, n_send;
    socklen_t   len;

    ret = client_connect(ss);
    if(ret != 0) {
        return -1;
    }//end if

    memset(&ss, 0, sizeof(ss));
    len = sizeof(struct sockaddr_storage);

    /* sendto(), recvfrom() and echo */
    n_send = sendto(client_fd, send_message, strlen(send_message) + 1, 0, (struct sockaddr *)ss, len);
    if(n_send < 0) {
        fprintf(stderr, "sendto(): %s\n", strerror(errno));
        close(client_fd);
        return -1;
    }//end if

    lock = true;
    while(lock) {
        usleep(1000 * 50);
    }//end while

    printf("client: sendto(%zd bytes): %s, tid: %d\n", n_send, send_message, gettid());

    memset(buf, 0, sizeof(buf));
    n_recv = recvfrom(client_fd, buf, sizeof(buf), 0, (struct sockaddr *)&ss, &len);
    if(n_recv < 0) {
        fprintf(stderr, "recvfrom(): %s\n", strerror(errno));
        close(client_fd);
        return -1;
    }//end if
    printf("client: recvfrom(%zd bytes): %s, tid: %d\n", n_recv, buf, gettid());

    return 0;
}//end client_dgram_connect_and_echo
