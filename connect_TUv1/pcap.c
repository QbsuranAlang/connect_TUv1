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

pid_t start_tcpdump(const char *filename_prefix) {
    int     index, ret, out_fno, err_fno;
    char    filename[256], cmd[512], *argv[8];
    bool    found;
    pid_t   pid;

    found = false;
    for(index = 0; index < 999; index++) {
        snprintf(filename, sizeof(filename), "%s-%03d.pcap", filename_prefix, index);
        if(file_is_exist(filename)) {
            found = true;
            continue;
        }//end if
        found = false;
        break;
    }//end while

    if(found) {
        fprintf(stderr, "filename: %s-%03d.pcap to %s-%03d.pcap is full\n", filename_prefix, 0, filename_prefix, 999);
        return (pid_t)-1;
    }//end if

    argv[0] = "/bin/sh";
    argv[1] = "-c";
    argv[2] = cmd;
    snprintf(cmd, sizeof(cmd), "echo $$ && exec tcpdump -i lo -nn port %d -w %s", server_port, filename);
    argv[3] = NULL;

    printf("Running tcpdump in backgroud...\n");
    printf("pcap filename: %s\n", filename);
    ret = spawn_async(&pid, &out_fno, &err_fno, argv, NULL);
    if(ret != 0) {
        return (pid_t)-1;
    }//end if

    sleep(1);
    if(out_fno >= 0) {
        close(out_fno);
    }//end if

    if(err_fno >= 0) {
        close(err_fno);
    }//end if

    return (pid_t)pid;
}//end start_tcpdump

void stop_tcpdump(pid_t pid) {
    int ret;

    printf("Stopping tcpdump...\n");
    if(sock_type != SOCK_DGRAM) {
        sleep(5);
    } else {
        sleep(1);
    }//end else
    kill(pid, SIGTERM);
    NO_EINTR(ret = waitpid(pid, NULL, 0));

    ret = kill(pid, 0);
    if(ret != 0) {
        kill(pid, SIGKILL);
    }//end if
}//end stop_tcpdump
