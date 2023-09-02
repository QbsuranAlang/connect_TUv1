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

typedef enum {
    FILE_TEST_IS_REGULAR    = 1 << 0,
    FILE_TEST_IS_SYMLINK    = 1 << 1,
    FILE_TEST_IS_DIR        = 1 << 2,
    FILE_TEST_IS_EXECUTABLE = 1 << 3,
    FILE_TEST_IS_EXISTS     = 1 << 4
} e_file_test_t;

bool file_is_exist(const char *filename) {
    int         test;
    struct stat s;

    test = FILE_TEST_IS_REGULAR | FILE_TEST_IS_SYMLINK | FILE_TEST_IS_EXISTS;

    if((test & FILE_TEST_IS_EXISTS) && (access(filename, F_OK) == 0)) {
        return true;
    }//end if

    if((test & FILE_TEST_IS_EXECUTABLE) && (access(filename, X_OK) == 0)) {
        if(getuid() != 0) {
            return true;
        }//end if

        /*
         * for root, on some POSIX systems, access (filename, X_OK)
         * will succeed even if no executable bits are set on the
         * file. We fall through to a stat test to avoid that.
         */
    }//end if
    else {
        test &= ~FILE_TEST_IS_EXECUTABLE;
    }//end else

    if(test & FILE_TEST_IS_SYMLINK) {
        if((lstat(filename, &s) == 0) && S_ISLNK(s.st_mode)) {
            return true;
        }//end if
    }//end if

    if(test & (FILE_TEST_IS_REGULAR | FILE_TEST_IS_DIR | FILE_TEST_IS_EXECUTABLE)) {
        if(stat(filename, &s) == 0) {
            if((test & FILE_TEST_IS_REGULAR) && S_ISREG(s.st_mode)) {
                return true;
            }//end if

            if((test & FILE_TEST_IS_DIR) && S_ISDIR(s.st_mode)) {
                return true;
            }//end if

            /* The extra test for root when access (file, X_OK) succeeds. */
            if((test & FILE_TEST_IS_EXECUTABLE) && ((s.st_mode & S_IXOTH) || (s.st_mode & S_IXUSR) || (s.st_mode & S_IXGRP))) {
                return true;
            }//end if
        }//end if
    }//end if

    return false;
}//end file_is_exist
