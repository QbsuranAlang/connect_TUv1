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

#define TARGET_NUM 5

static void usage(const char *cmd) {
    fprintf(stderr, "%s [-c client port] [-h help]\n", cmd);
    fprintf(stderr, "%s -c 12345\n", cmd);
    exit(1);
}//end usage

static int connect_a_target(const char *ip_addr, unsigned short client_port) {
    int                     fd, ret;
    struct timeval          tv;
    struct sockaddr_in      *in;
    struct sockaddr_storage ss;

    /* open socket */
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        fprintf(stderr, "socket(): %s\n", strerror(errno));
        exit(1);
    }//end if

    /* set timeout */
    tv.tv_sec = 1;
    tv.tv_usec = 500;

    ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(SO_RCVTIMEO): %s\n", strerror(errno));
        exit(1);
    }//end if

    ret = setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(struct timeval));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(SO_SNDTIMEO): %s\n", strerror(errno));
        exit(1);
    }//end if

    /* fill struct sockaddr */
    memset(&ss, 0, sizeof(struct sockaddr_storage));
    in = (struct sockaddr_in *)&ss;
    in->sin_family = PF_INET;
    in->sin_port = htons(80);

    ret = inet_pton(PF_INET, ip_addr, &in->sin_addr);
    if(ret == 0) {
        errno = EINVAL;
        fprintf(stderr, "inet_pton(): %s\n", strerror(errno));
        exit(1);
    } else if(ret == -1) {
        fprintf(stderr, "inet_pton(): %s\n", strerror(errno));
        exit(1);
    }//end if

    /* start connect_TUv1() */
    printf("Start connecting to %s\n", ip_addr);
    ret = connect_TUv1(fd, (struct sockaddr *)&ss, sizeof(struct sockaddr_storage), client_port);
    if(ret != 0) {
        fprintf(stderr, "connect_TUv1(): %s\n", strerror(errno));
        exit(1);
    }//end if

    return 0;
}//end connect_a_target

static void start_multi_connect(in_port_t client_port) {
    int         i, fd4[TARGET_NUM];
    char        cmd[256];
    const char  *target4[TARGET_NUM];

    /* ipv4 */
    target4[0] = "1.1.1.1";
    target4[1] = "1.0.0.1";
    target4[2] = "31.13.77.35";
    target4[3] = "172.217.163.36";
    target4[4] = "142.251.42.238";

    /* connect to ipv4 */
    for(i = 0; i < TARGET_NUM; i++) {
        fd4[i] = connect_a_target(target4[i], client_port);
    }//end for

    /* show connection */
    snprintf(cmd, sizeof(cmd), "ss -ant | grep %d", client_port);
    printf("\nCommand: %s\n", cmd);
    system(cmd);

    /* close connection */
    for(i = 0; i < TARGET_NUM; i++) {
        close(fd4[i]);
    }//end for
}//end start_multi_connect

int main(int argc, char *argv[]) {
    int c, client_port;

    client_port = 0;
    while((c = getopt(argc, argv, "c:h")) != EOF) {
        switch(c) {
        case 'c':
            client_port = atoi(optarg);
        break;

        case 'h':
        default:
            usage(argv[0]);
        }//end switch
    }//end while

    if(client_port == 0 || client_port <= 0 || client_port >= 65536) {
        usage(argv[0]);
    }//end if

    start_multi_connect(client_port);
    return 0;
}//end main
