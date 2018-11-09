CC=gcc
CFLAGS=-I./include -I./libxsvf -g
LDFLAGS_COMMON=$(shell pkg-config --libs libusb-1.0)
LDFLAGS_TEST= -L./build -llogsys-drv

OBJS_SO=build/tmp/shared/usb.o build/tmp/shared/status.o build/tmp/shared/jconf.o

all: build/liblogsys-drv.so

test: build/logsys-test build/hotplug-test

clean:
	find build -type f -delete

build/liblogsys-drv.so: $(OBJS_SO) libxsvf/libxsvf.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) --shared -o $@

build/logsys-test: build/tmp/test/usbtest.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/hotplug-test: build/tmp/test/hotplug.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/tmp/shared/%.o: src/shared/%.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

build/tmp/test/%.o: src/test/%.c
	$(CC) $(CFLAGS) -c $< -o $@
