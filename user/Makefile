PROGS = shell ls shutdown echo cat TestProgram login

all : $(PROGS)

-include ../common.mak

shell : CCFILES=
shell : CFILES=shell.c libc.c heap.c

echo : CCFILES=
echo : CFILES=echo.c libc.c heap.c

ls : CCFILES=
ls : CFILES=ls.c libc.c heap.c

shutdown : CCFILES=
shutdown : CFILES=shutdown.c libc.c heap.c

cat : CCFILES=
cat : CFILES=cat.c libc.c heap.c

login: CCFILES=login.cc

login: CFILES=libc.c heap.c

TestProgram : CFILES=libc.c heap.c

TestProgram : CCFILES=TestProgram.cc DesktopWindowManager.cc

$(PROGS) : % : Makefile $(OFILES)
	ld -N -m elf_i386 -e start -Ttext=0x80000000 -o $@ $(OFILES)

clean ::
	rm -f $(PROGS)
