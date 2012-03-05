SRCS = syncookiepad.c
OBJS = $(SRCS:.c=.o)
CFLAGS += -Wall -Werror

syncookiepad: syncookiepad.o

syncookiepad-asm:
	$(CC) -g -c -Wa,-a,-ad -O2 syncookiepad.c > syncookiepad.lst

$(SRCS):
	$(CC) -o $* $*.c

clean:
	rm -vf -- $(OBJS) syncookiepad.lst

