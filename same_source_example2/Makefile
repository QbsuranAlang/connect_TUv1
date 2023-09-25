# MIT License
#
# Copyright (c) 2023 same_source
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#


CC = gcc
LINK = $(CC)
CFLAGS = -Wall -D_GNU_SOURCE
CORE_INCS = -I./

.PHONY: same_source main.o connect_TUv1.o clean distclean

same_source: main.o connect_TUv1.o
	$(LINK) -o same_source main.o connect_TUv1.o

main.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o main.o main.c

connect_TUv1.o:
	$(CC) -c $(CFLAGS) $(CORE_INCS) -o connect_TUv1.o connect_TUv1.c

clean:
	rm -rf *.o same_source

distclean:
	make clean
