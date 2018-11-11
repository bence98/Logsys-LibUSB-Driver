CC=gcc
LIBNAME=liblogsys-drv.so
#Release Candidate
MAJOR=0
#RC1
SUBVERSION=0.1
CFLAGS=-I./include -I./libxsvf -g
LDFLAGS_COMMON=$(shell pkg-config --libs libusb-1.0)
LDFLAGS_TEST=-L./build -llogsys-drv
LDFLAGS_SO=--shared -Wl,-soname,$(LIBNAME).$(MAJOR)

OBJS_SO=build/tmp/shared/usb.o build/tmp/shared/status.o build/tmp/shared/jconf.o

all: build/$(LIBNAME)

test: build/logsys-test build/hotplug-test

clean:
	find build -type f -delete

install: build/$(LIBNAME)
	cp $< /usr/local/lib/$(LIBNAME).$(MAJOR).$(SUBVERSION)
	ldconfig /usr/local/lib
	ln -sf /usr/local/lib/$(LIBNAME).$(MAJOR) /usr/local/lib/$(LIBNAME)

build/$(LIBNAME): $(OBJS_SO) libxsvf/libxsvf.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_SO) -o $@

build/logsys-test: build/tmp/test/usbtest.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/hotplug-test: build/tmp/test/hotplug.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/tmp/shared/%.o: src/shared/%.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

build/tmp/test/%.o: src/test/%.c
	$(CC) $(CFLAGS) -c $< -o $@
