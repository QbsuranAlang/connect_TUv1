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

int sctp_server_setup(void) {
    int                     ret;
    struct sctp_initmsg     initmsg;
    struct sockaddr_storage ss;

    /* common socket setup */
    ret = server_setup_common(&ss);
    if(ret == -1) {
        return -1;
    }//end if

    /* bind */
    ret = server_bind_and_listen_common(&ss);
    if(ret == -1) {
        return -1;
    }//end if

    /* specify that a maximum of 5 streams will be available per socket */
    memset(&initmsg, 0, sizeof (initmsg));
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts = 4;
    ret = setsockopt(server_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(SCTP_INITMSG): %s\n", strerror(errno));
        return -1;
    }//end if

    return 0;
}//end sctp_server_setup

void *sctp_main_server(void *arg) {
    int ret;

    stop = false;
    while(!stop) {
        ret = server_main_stream_server_accept4_and_echo_common();
        if(ret != 0) {
            break;
        }//end if
        stop = true;
    }//end while

    pthread_exit(NULL);
}//end tcp_main_server

void stop_sctp_server(void) {
    stop = true;
    if(tid != -1) {
        pthread_join(tid, NULL);
    }//end if
    tid = -1;
    printf("SCTP server is stopped\n");
}//end stop_sctp_server
