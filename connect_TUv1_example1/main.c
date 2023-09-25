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

struct proto_ops ops[] = {
    {
        .proto = "TCP",
        .send_message_v4 = TEXT_TCP_IPv4,
        .send_message_v6 = TEXT_TCP_IPv6,
        .server_setup = tcp_server_setup,
        .main_server = tcp_main_server,
        .stop_server = stop_tcp_server,
        .client_jobs = tcp_client_jobs,
    },
    {
        .proto = "UDP",
        .send_message_v4 = TEXT_UDP_IPv4,
        .send_message_v6 = TEXT_UDP_IPv6,
        .server_setup = udp_server_setup,
        .main_server = udp_main_server,
        .stop_server = stop_udp_server,
        .client_jobs = udp_client_jobs,
    },
    {
        .proto = "SCTP",
        .send_message_v4 = TEXT_SCTP_IPv4,
        .send_message_v6 = TEXT_SCTP_IPv6,
        .server_setup = sctp_server_setup,
        .main_server = sctp_main_server,
        .stop_server = stop_sctp_server,
        .client_jobs = sctp_client_jobs,
    },
    {
        .proto = "DCCP",
        .send_message_v4 = TEXT_DCCP_IPv4,
        .send_message_v6 = TEXT_DCCP_IPv6,
        .server_setup = dccp_server_setup,
        .main_server = dccp_main_server,
        .stop_server = stop_dccp_server,
        .client_jobs = dccp_client_jobs,
    },
};

static void usage(const char *cmd) {
    fprintf(stderr, "%s [-4/-6](IP version) [-p tcp/udp/sctp/dccp](IP protocol) [-c client port] [old/new](-n connect()/connect_TUv1()) [-w save to pcap] [-h help]\n", cmd);
    fprintf(stderr, "%s -4 -p tcp -c 12345, call connect() with source port is 12345\n", cmd);
    fprintf(stderr, "%s -4 -p tcp -c 12345 -n -w, call connect_TUv1() with source port is 12345\n", cmd);
    exit(1);
}//end usage

int main(int argc, char *argv[]) {
    int         c, ret, i;
    pid_t       pid;

    init_rand();

    opterr = 0;
    while((c = getopt(argc, argv, "46c:p:nwh")) != EOF) {
        switch(c) {
        case '4':
            if(sa_family != 0) {
                fprintf(stderr, "%s option IP version is duplicated\n", argv[0]);
                exit(1);
            }//end if
            sa_family = PF_INET;
        break;

        case '6':
            if(sa_family != 0) {
                fprintf(stderr, "%s option IP version is duplicated\n", argv[0]);
                exit(1);
            }//end if
            sa_family = PF_INET6;
        break;

        case 'c':
            client_port = atoi(optarg);
        break;

        case 'p':
            if(ip_proto != 0) {
                fprintf(stderr, "%s option IP protocol is duplicated\n", argv[0]);
                exit(1);
            }//end if
            if(strcasecmp(optarg, "tcp") == 0) {
                ip_proto = IPPROTO_TCP;
                sock_type = SOCK_STREAM;
                ip_proto_str = "TCP";
            } else if(strcasecmp(optarg, "udp") == 0) {
                ip_proto = IPPROTO_UDP;
                sock_type = SOCK_DGRAM;
                ip_proto_str = "UDP";
            } else if(strcasecmp(optarg, "sctp") == 0) {
                ip_proto = IPPROTO_SCTP;
                sock_type = SOCK_STREAM;
                ip_proto_str = "SCTP";
            } else if(strcasecmp(optarg, "dccp") == 0) {
                ip_proto = IPPROTO_DCCP;
                sock_type = SOCK_DCCP;
                ip_proto_str = "DCCP";
            } else {
                usage(argv[0]);
            }//end else
        break;

        case 'n':
            use_old = false;
        break;

        case 'w':
            save = true;
        break;

        case 'h':
        default:
            usage(argv[0]);
        }//end switch
    }//end while

    if(sa_family == PF_UNSPEC ||
        ip_proto == IPPROTO_IP || sock_type == SOCK_RAW ||
        client_port == 0 ||
        ip_proto_str == NULL) {
        usage(argv[0]);
    }//end if

    if(use_old && client_port != 0) {
        fprintf(stderr, "Warning: client port (-c)option is not working when using connect()(without -n option)\n");
    }//end if

    /* open server and client fd */
    server_fd = socket(sa_family, sock_type, ip_proto);
    if(server_fd < 0) {
        fprintf(stderr, "socket(): %s\n", strerror(errno));
        exit(1);
    }//end if

    client_fd = socket(sa_family, sock_type, ip_proto);
    if(client_fd < 0) {
        fprintf(stderr, "socket(): %s\n", strerror(errno));
        exit(1);
    }//end if

    /* random a server port */
    server_port = rand_a_port();
    printf("Random a server port: %d\n", server_port);

    /* start */
    for(i = 0 ; i < sizeof(ops)/sizeof(ops[0]); i++) {
        if(!strcasecmp(ops[i].proto, ip_proto_str)) {

            if(save) {
                pid = start_tcpdump(ops[i].proto);
                if(pid == (pid_t)-1) {
                    exit(1);
                }//end if
            }//end if

            ret = ops[i].server_setup();
            if(ret != 0) {
                exit(1);
            }//end if

            ret = start_server(ops[i].proto, ops[i].main_server);
            if(ret != 0) {
                exit(1);
            }//end if

            if(sa_family == PF_INET) {
                ret = ops[i].client_jobs(ops[i].send_message_v4);
            } else if(sa_family == PF_INET6) {
                ret = ops[i].client_jobs(ops[i].send_message_v6);
            } else {
                fprintf(stderr, "%s: %s(), %d: never should be here\n", __FILE__, __func__, __LINE__);
                ret = -1;
            }//end else
            if(ret != 0) {
                exit(1);
            }//end if

            ops[i].stop_server();

            if(save) {
                stop_tcpdump(pid);
            }//end if
            break;
        }//end if
    }//end for

    /* cleanup */
    if(server_fd >= 0) {
        close(server_fd);
    }//end if
    if(client_fd >= 0) {
        close(client_fd);
    }//end if

    return 0;
}//end main
