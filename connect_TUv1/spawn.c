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

int spawn_async(pid_t *child_pid, int *out_fno, int *err_fno, char *argv[], char *envp[]) {
    int                         ret, out_p[2], err_p[2];
    posix_spawn_file_actions_t  action;

    if(out_fno && pipe(out_p) != 0) {
        fprintf(stderr, "pipe(): %s\n", strerror(errno));
        return -1;
    }//end if

    if(err_fno && pipe(err_p) != 0) {
        fprintf(stderr, "pipe(): %s\n", strerror(errno));
        if(out_fno) {
            close(out_p[0]);
            close(out_p[1]);
        }//end if
        return -1;
    }//end if

    ret = posix_spawn_file_actions_init(&action);
    if(ret != 0) {
        fprintf(stderr, "posix_spawn_file_actions_init(): %s\n", strerror(ret));
        if(out_fno) {
            close(out_p[0]);
            close(out_p[1]);
        }//end if
        if(err_fno) {
            close(err_p[0]);
            close(err_p[1]);
        }//end if
        return -1;
    }//end if

    if(out_fno) {
        ret = posix_spawn_file_actions_addclose(&action, out_p[0]);
        if(ret != 0) {
            fprintf(stderr, "posix_spawn_file_actions_addclose(): %s\n", strerror(ret));
            posix_spawn_file_actions_destroy(&action);
            if(out_fno) {
                close(out_p[0]);
                close(out_p[1]);
            }//end if
            if(err_fno) {
                close(err_p[0]);
                close(err_p[1]);
            }//end if
            return -1;
        }//end if

        ret = posix_spawn_file_actions_adddup2(&action, out_p[1], STDOUT_FILENO);
        if(ret != 0) {
            fprintf(stderr, "posix_spawn_file_actions_adddup2(): %s\n", strerror(ret));
            posix_spawn_file_actions_destroy(&action);
            if(out_fno) {
                close(out_p[0]);
                close(out_p[1]);
            }//end if
            if(err_fno) {
                close(err_p[0]);
                close(err_p[1]);
            }//end if
            return -1;
        }//end if

        ret = posix_spawn_file_actions_addclose(&action, out_p[1]);
        if(ret != 0) {
            fprintf(stderr, "posix_spawn_file_actions_adddup2(): %s\n", strerror(ret));
            posix_spawn_file_actions_destroy(&action);
            if(out_fno) {
                close(out_p[0]);
                close(out_p[1]);
            }//end if
            if(err_fno) {
                close(err_p[0]);
                close(err_p[1]);
            }//end if
            return -1;
        }//end if
    }//end if

    if(err_fno) {
        ret = posix_spawn_file_actions_addclose(&action, err_p[0]);
        if(ret != 0) {
            fprintf(stderr, "posix_spawn_file_actions_addclose(): %s\n", strerror(ret));
            posix_spawn_file_actions_destroy(&action);
            if(out_fno) {
                close(out_p[0]);
                close(out_p[1]);
            }//end if
            if(err_fno) {
                close(err_p[0]);
                close(err_p[1]);
            }//end if
            return -1;
        }//end if

        ret = posix_spawn_file_actions_adddup2(&action, err_p[1], STDERR_FILENO);
        if(ret != 0) {
            fprintf(stderr, "posix_spawn_file_actions_adddup2(): %s\n", strerror(ret));
            posix_spawn_file_actions_destroy(&action);
            if(out_fno) {
                close(out_p[0]);
                close(out_p[1]);
            }//end if
            if(err_fno) {
                close(err_p[0]);
                close(err_p[1]);
            }//end if
            return -1;
        }//end if

        ret = posix_spawn_file_actions_addclose(&action, err_p[1]);
        if(ret != 0) {
            fprintf(stderr, "posix_spawn_file_actions_addclose(): %s\n", strerror(ret));
            posix_spawn_file_actions_destroy(&action);
            if(out_fno) {
                close(out_p[0]);
                close(out_p[1]);
            }//end if
            if(err_fno) {
                close(err_p[0]);
                close(err_p[1]);
            }//end if
            return -1;
        }//end if
    }//end if

    ret = posix_spawnp(child_pid, argv[0], &action, NULL, argv, envp);
    if(ret != 0) {
        fprintf(stderr, "posix_spawnp(): %s\n", strerror(ret));
        posix_spawn_file_actions_destroy(&action);
        if(out_fno) {
            close(out_p[0]);
            close(out_p[1]);
        }//end if
        if(err_fno) {
            close(err_p[0]);
            close(err_p[1]);
        }//end if
        return -1;
    }//end if

    if(out_fno) {
        close(out_p[1]);
        *out_fno = out_p[0];
    }//end if
    if(err_fno) {
        close(err_p[1]);
        *err_fno = err_p[0];
    }//end if

    posix_spawn_file_actions_destroy(&action);
    return 0;
}//end spawn_async

int spawn_sync(pid_t *child_pid, int *status, int *out_fno, int *err_fno, char *argv[], char *envp[]) {
    int     ret;
    pid_t   pid;

    ret = spawn_async(&pid, out_fno, err_fno, argv, envp);
    if(ret != 0) {
        return -1;
    }//end if

    if(child_pid) {
        *child_pid = pid;
    }//end if

    NO_EINTR(ret = waitpid(pid, status, 0));
    if(ret < 0) {
        fprintf(stderr, "waitpid(%d): %s\n", pid, strerror(errno));
        if(out_fno) {
            close(*out_fno);
            *out_fno = -1;
        }//end if
        if(err_fno) {
            close(*err_fno);
            *err_fno = -1;
        }//end if
        return -1;
    }//end if

    return 0;
}//end spawn_sync
