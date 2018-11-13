UNAME=$(shell uname -s)
CC=gcc
LIBNAME=liblogsys-drv.so
#Release Candidate
MAJOR=0
#RC1
SUBVERSION=0.1
CFLAGS=-I./include -I./libxsvf -g
LDFLAGS_COMMON=$(shell pkg-config --libs libusb-1.0)
LDFLAGS_TEST=-L./build -llogsys-drv
ifeq ($(UNAME), Linux)
LDFLAGS_SO=--shared -Wl,-soname,$(LIBNAME).$(MAJOR)
endif
ifeq ($(UNAME), Darwin)
LDFLAGS_SO=--shared -Wl,-install_name,$(LIBNAME).$(MAJOR)
endif

OBJS_SO=build/tmp/shared/usb.o build/tmp/shared/status.o build/tmp/shared/jconf.o

all: build/$(LIBNAME)

test: build/logsys-test build/hotplug-test

clean:
	rm -rf build
#	find build -type f -delete

install: build/$(LIBNAME)
	cp $< /usr/local/lib/$(LIBNAME).$(MAJOR).$(SUBVERSION)
ifeq ($(UNAME), Linux)
	ldconfig /usr/local/lib
endif
ifeq ($(UNAME), Darwin)
	ln -sf /usr/local/lib/$(LIBNAME).$(MAJOR).$(SUBVERSION) /usr/local/lib/$(LIBNAME).$(MAJOR)
endif
	ln -sf /usr/local/lib/$(LIBNAME).$(MAJOR) /usr/local/lib/$(LIBNAME)

deb: build/logsys-drv.deb

libxsvf/libxsvf.a:
	cd libxsvf; $(MAKE) $(notdir $@)

build/logsys-drv.deb: build/$(LIBNAME) build/logsys-test
	mkdir -p build/logsys-drv/usr/local/lib/
	mkdir -p build/logsys-drv/usr/local/bin/
	mkdir -p build/logsys-drv/etc/udev/rules.d/
	cp -r DEBIAN build/logsys-drv/
	cp $< build/logsys-drv/usr/local/lib/$(LIBNAME).$(MAJOR).$(SUBVERSION)
	cd build/logsys-drv/usr/local/lib/; ln -s $(LIBNAME).$(MAJOR) $(LIBNAME)
	cp $(lastword $^) build/logsys-drv/usr/local/bin/
	./udev-rule.sh build/logsys-drv
	cd build; dpkg-deb --build logsys-drv

build/$(LIBNAME): $(OBJS_SO) libxsvf/libxsvf.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_SO) -o $@

build/logsys-test: build/tmp/test/usbtest.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/hotplug-test: build/tmp/test/hotplug.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/tmp/shared/%.o: src/shared/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

build/tmp/test/%.o: src/test/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
