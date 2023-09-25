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

int dccp_client_jobs(const char *send_message) {
    int                     ret, optval;
    uint32_t                mps;
    socklen_t               res_len;
    struct sockaddr_storage ss;

    ret = client_setup_common(&ss);
    if(ret != 0) {
        return -1;
    }//end if

    optval = htonl(DCCP_SERVICE_CODE);
    ret = setsockopt(client_fd, SOL_DCCP, DCCP_SOCKOPT_SERVICE, &optval, sizeof(int));
    if(ret != 0) {
        fprintf(stderr, "setsockopt(DCCP_SOCKOPT_SERVICE): %s\n", strerror(errno));
        return -1;
    }//end if

    res_len = sizeof(uint32_t);
    ret = getsockopt(client_fd, SOL_DCCP, DCCP_SOCKOPT_GET_CUR_MPS, &mps, &res_len);
    if(ret != 0) {
        fprintf(stderr, "getsockopt(DCCP_SOCKOPT_GET_CUR_MPS): %s\n", strerror(errno));
        return -1;
    }//end if
    printf("Maximum DCCP Packet Size: %d bytes\n", mps);

    ret = client_stream_connect_and_echo(&ss, send_message);
    if(ret != 0) {
        return -1;
    }//end if

    return 0;
}//end dccp_client_jobs
