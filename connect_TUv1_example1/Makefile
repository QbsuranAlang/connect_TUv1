
CC = gcc
LINK = $(CC)
CFLAGS = -Wall -D_GNU_SOURCE
CORE_INCS = -I./
LIBS = -lsctp -lpthread -lrt

.PHONY: connect_TUv1 main.o connect_TUv1.o common.o pcap.o rand.o spawn.o fileutils.o tcp_server.o tcp_client.o udp_server.o udp_client.o sctp_client.o sctp_server.o dccp_client.o dccp_server.o clean distclean

connect_TUv1: main.o connect_TUv1.o common.o pcap.o rand.o spawn.o fileutils.o tcp_server.o tcp_client.o udp_server.o udp_client.o sctp_client.o sctp_server.o dccp_client.o dccp_server.o
	$(LINK) -o connect_TUv1 main.o connect_TUv1.o common.o spawn.o fileutils.o pcap.o rand.o tcp_server.o tcp_client.o udp_server.o udp_client.o sctp_client.o sctp_server.o dccp_client.o dccp_server.o $(LIBS)

main.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o main.o main.c

connect_TUv1.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o connect_TUv1.o connect_TUv1.c

common.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o common.o common.c

pcap.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o pcap.o pcap.c

rand.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o rand.o rand.c

spawn.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o spawn.o spawn.c

fileutils.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o fileutils.o fileutils.c

tcp_server.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o tcp_server.o tcp_server.c

tcp_client.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o tcp_client.o tcp_client.c

udp_server.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o udp_server.o udp_server.c

udp_client.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o udp_client.o udp_client.c

sctp_server.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o sctp_server.o sctp_server.c

sctp_client.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o sctp_client.o sctp_client.c

dccp_server.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o dccp_server.o dccp_server.c

dccp_client.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o dccp_client.o dccp_client.c

clean:
	rm -rf *.o *.pcap connect_TUv1

distclean:
	make clean
