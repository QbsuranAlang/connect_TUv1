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


#ifndef DCCP_H
#define DCCP_H

/* From the kernel's include/linux/socket.h */
#define SOL_DCCP                        269

/* From kernel's include/uapi/linux/dccp.h */
#define DCCP_SOCKOPT_SERVICE            2
#define DCCP_SOCKOPT_CHANGE_L           3
#define DCCP_SOCKOPT_CHANGE_R           4
#define DCCP_SOCKOPT_GET_CUR_MPS        5
#define DCCP_SOCKOPT_SERVER_TIMEWAIT    6
#define DCCP_SOCKOPT_SEND_CSCOV         10
#define DCCP_SOCKOPT_RECV_CSCOV         11
#define DCCP_SOCKOPT_AVAILABLE_CCIDS    12
#define DCCP_SOCKOPT_CCID               13
#define DCCP_SOCKOPT_TX_CCID            14
#define DCCP_SOCKOPT_RX_CCID            15
#define DCCP_SOCKOPT_QPOLICY_ID         16
#define DCCP_SOCKOPT_QPOLICY_TXQLEN     17
#define DCCP_SOCKOPT_CCID_RX_INFO       128
#define DCCP_SOCKOPT_CCID_TX_INFO       192

#endif /* DCCP_H */
