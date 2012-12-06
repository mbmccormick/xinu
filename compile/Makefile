#
#  Makefile for the Xinu operating system.
#

MIPS_ROOT = /p/xinu/mips-dev/bin
MIPS_PREFIX = mips-

COMPILER_ROOT = ${MIPS_ROOT}/${MIPS_PREFIX}

CC       = ${COMPILER_ROOT}gcc
LD       = ${COMPILER_ROOT}ld
OBJCOPY  = ${COMPILER_ROOT}objcopy
MAKEDEP  = gcc -M -MG

INCLUDE  = -I../include 

# C compilation flags
CFLAGS = -O0 -Wall -Werror -Wstrict-prototypes -Wno-trigraphs		\
	     -nostdinc -fno-builtin -fno-strict-aliasing -fno-common	\
       	 -fomit-frame-pointer -fno-pic -ffunction-sections -G 0		\
         -mlong-calls -mno-abicalls -mabi=32 -march=mips32r2		\
         -Wa,-32 -Wa,-march=mips32r2 -Wa,-mips32r2 -Wa,--trap		\
         ${INCLUDE} -c

# Assembler flags
ASFLAGS  = ${INCLUDE} -march=mips32r2 -mips32r2

# Loader flags
LDFLAGS   = -static --gc-sections -T ld.script

# Objcopy flags
OCFLAGS  = -I binary -O elf32-bigmips -B mips

MKIMAGE   = uboot-tool/mkimage
UBOOTOPTS = -A mips -O linux -T kernel -a 0x80010000 -C none -e 0x80010000 -n 'Xinu'

xinu.boot: xinu.bin ${MKIMAGE}
	${MKIMAGE} ${UBOOTOPTS} -d xinu.bin $@

xinu.bin: xinu.elf
	${OBJCOPY} -O binary $^ $@

TOPDIR    = ..

COMPS     = system device/tty device/eth device/flash device/rfs shell \
		net device/rds device/lfs device/ram device/nam

# Include expected files, each should add its part to the compile source
COMP_SRC :=
include ${COMPS:%=${TOPDIR}/%/Makerules}

COMP_OBJ = $(patsubst %.S,%.o,$(filter %.S,$(COMP_SRC))) \
           $(patsubst %.c,%.o,$(filter %.c,$(COMP_SRC)))

# Export variables for recursive make calls (such as the library)
export
################
# Make targets #
################
xinu.elf: ${COMP_OBJ} ../lib/libxc.a
	${LD} -o $@ ${LDFLAGS} $^

vdump:
	@echo
	@echo "COMP_OBJ is $(COMP_OBJ)"
	@echo
	@echo "COMP_SRC is $(COMP_SRC)"
	@echo

objects: ${COMP_OBJ}

clean:
	rm -f *.o ${COMP_OBJ}
	rm -f xinu.boot xinu.bin xinu.elf
	rm -rf ${DOCS}

depend:
	$(MAKEDEP) ${INCLUDE} ${COMP_SRC} > .deps

${MKIMAGE}:
	@echo "ERROR: The U-Boot utilities are not part of Xinu."
	@echo "ERROR: Please acquire the mkimage utility separately."
	exit 1

debug: 
	$(MAKE) DEBUG="-DDEBUG ${BUGFLAG}"

depclean:
	rm -f .deps
	touch .deps

realclean: clean depclean
	rm -f vn version
	rm -f Makefile.bak

###################
# Special targets #
###################
${COMPS}:
	$(MAKE) COMPS="$@" objects

include .deps

# Special rules for special files

../system/initialize.o: ../system/initialize.c
	@sh mkvers.sh
	@VERS=`cat version`
	@${CC} -DVERSION=\""`cat version`"\" ${CFLAGS} -o $@ $<

../lib/libxc.a:							\
	../lib/libxc/abs.c     ../lib/libxc/atoi.c    ../lib/libxc/atol.c \
	../lib/libxc/bzero.c   ../lib/libxc/ctype_.c  ../lib/libxc/doprnt.c \
	../lib/libxc/doscan.c  ../lib/libxc/fdoprnt.c ../lib/libxc/fgetc.c \
	../lib/libxc/fgets.c   ../lib/libxc/fprintf.c ../lib/libxc/fputc.c \
	../lib/libxc/fputs.c   ../lib/libxc/fscanf.c  ../lib/libxc/getchar.c \
	../lib/libxc/labs.c    ../lib/libxc/memchr.c  ../lib/libxc/memcmp.c \
	../lib/libxc/memcpy.c  ../lib/libxc/memset.c  ../lib/libxc/printf.c \
	../lib/libxc/putchar.c ../lib/libxc/qsort.c   ../lib/libxc/rand.c \
	../lib/libxc/sprintf.c ../lib/libxc/sscanf.c  ../lib/libxc/strchr.c \
	../lib/libxc/strncat.c ../lib/libxc/strncmp.c ../lib/libxc/strncpy.c \
	../lib/libxc/strnlen.c ../lib/libxc/strrchr.c ../lib/libxc/strstr.c
	@ echo making library
	@(cd ../lib/libxc; make install)
