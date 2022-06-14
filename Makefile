UNAME=$(shell uname -s)
ifeq ($(UNAME), Darwin)
OS=macOS
else ifeq ($(UNAME), Linux)
OS=Linux
else ifneq ($(findstring MINGW, $(UNAME)),)
OS=Windows
else ifneq ($(findstring CYGWIN, $(UNAME)),)
OS=Windows
else ifeq ($(UNAME), Haiku)
OS=Haiku
else
OS=N/A
endif

#Release Candidate
MAJOR=0
#RC1
SUBVERSION=0.1

CFLAGS=-I./include -I./libxsvf -g
CXXFLAGS=-I./include -g
LDFLAGS_TEST=-L./build -llogsys-drv
LDFLAGS_TESTXX=-L./build -llogsyspp -llogsys-drv

ifeq ($(OS), macOS)
LIBNAME=liblogsys-drv.dylib
LDFLAGS_SO=--shared -Wl,-install_name,$(LIBNAME).$(MAJOR)
else ifeq ($(OS), Windows)
LIBNAME=liblogsys-drv.dll
LDFLAGS_SO=-shared -Wl,--out-implib,build/$(LIBNAME).a
else
LIBNAME=liblogsys-drv.so
LDFLAGS_SO=--shared -Wl,-soname,$(LIBNAME).$(MAJOR)
endif

LDFLAGS_COMMON=$(shell pkg-config --libs libusb-1.0)
ifeq ($(LDFLAGS_COMMON),)
LDFLAGS_COMMON=-lusb-1.0
endif

LIBNAME_CPP=liblogsyspp.a

SRCS_SHARED=$(notdir $(wildcard src/shared/*))
OBJS_SO=$(foreach src, $(SRCS_SHARED), build/tmp/shared/$(basename $(src)).o)

SRCS_CPP=$(notdir $(wildcard src/cpp/*))
OBJS_CPP=$(foreach src, $(SRCS_CPP), build/tmp/cpp/$(basename $(src)).o)

all: build/$(LIBNAME) build/$(LIBNAME_CPP)

test: build/logsys-test build/hotplug-test build/serio-test build/cpp-test

clean:
	rm -rf build
	cd libxsvf && make clean

install: build/$(LIBNAME)
	cp $< /usr/local/lib/$(LIBNAME).$(MAJOR).$(SUBVERSION)
ifeq ($(OS), Linux)
	ldconfig /usr/local/lib
else ifeq ($(OS), macOS)
	ln -sf /usr/local/lib/$(LIBNAME).$(MAJOR).$(SUBVERSION) /usr/local/lib/$(LIBNAME).$(MAJOR)
endif
	ln -sf /usr/local/lib/$(LIBNAME).$(MAJOR) /usr/local/lib/$(LIBNAME)

prep-pkg: build/$(LIBNAME) build/$(LIBNAME_CPP) build/logsys-test
	mkdir -p $(DESTDIR)/usr/lib/
	mkdir -p $(DESTDIR)/usr/bin/
	mkdir -p $(DESTDIR)/lib/udev/rules.d/
	cp -R include $(DESTDIR)/usr/include
	cp libxsvf/*.h $(DESTDIR)/usr/include
	cp $< $(DESTDIR)/usr/lib/$(LIBNAME).$(MAJOR).$(SUBVERSION)
	cd $(DESTDIR)/usr/lib/; ln -s $(LIBNAME).$(MAJOR) $(LIBNAME); ln -s $(LIBNAME).$(MAJOR).$(SUBVERSION) $(LIBNAME).$(MAJOR)
	cp $(word 2,$^) $(DESTDIR)/usr/lib/
	cp $(word 3,$^) $(DESTDIR)/usr/bin/
	./udev-rule.sh $(DESTDIR)

libxsvf/libxsvf.a:
	cd libxsvf; $(MAKE) $(notdir $@)

build/$(LIBNAME): $(OBJS_SO) libxsvf/libxsvf.a
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_SO) -o $@

build/$(LIBNAME_CPP): $(OBJS_CPP)
	$(AR) rc $@ -- $^

build/logsys-test: build/tmp/test/usbtest.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/hotplug-test: build/tmp/test/hotplug.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/serio-test: build/tmp/test/sio_fb.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/usart-test: build/tmp/test/uarttest.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS_COMMON) $(LDFLAGS_TEST) -o $@

build/cpp-test: build/tmp/test/cpptest.cpp.o
	$(CXX) $(CXXFLAGS) $^ -lstdc++ $(LDFLAGS_COMMON) $(LDFLAGS_TESTXX) -o $@

build/tmp/shared/%.o: src/shared/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -fPIC $< -o $@

build/tmp/cpp/%.o: src/cpp/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -fPIC $< -o $@

build/tmp/test/%.o: src/test/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

build/tmp/test/%.cpp.o: src/test/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@
