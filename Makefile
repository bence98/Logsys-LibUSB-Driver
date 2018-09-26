CC=gcc
CFLAGS=-I./include -g
LDFLAGS=$(shell pkg-config --libs libusb-1.0)

OBJS_SO=build/tmp/shared/logsys-usb.o

all: build/logsys-drv.so

test: build/logsys-test build/hotplug-test

clean:
	rm -f build/*/*
	rm -f build/*

build/logsys-drv.so: $(OBJS_SO)
	$(CC) $(CFLAGS) $^ --shared -o $@

build/logsys-test: build/tmp/test/usbtest.o build/logsys-drv.so
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

build/hotplug-test: build/tmp/test/hotplug.o build/logsys-drv.so
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

build/tmp/shared/%.o: src/shared/%.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

build/tmp/test/%.o: src/test/%.c
	$(CC) $(CFLAGS) -c $< -o $@
