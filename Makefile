
CSRCS = \
	tc8TestCase.cpp \
	TCP_BASICS_13.cpp \
	TCP_BASICS_14.cpp \
	TCP_CALL_ABORT_03_03.cpp \
	arp_proc.cpp \
	icmp_proc.cpp \
	proc_agv.cpp \
	main.cpp

STATICLIB_DIR=raw_sock_util
INCLUDE += -I$(STATICLIB_DIR)
STATICLIB = rawsock

APPNAME=tc8Test

OBJ_DIR = obj
vpath %.o $(OBJ_DIR)

CXXOBJS = $(patsubst %.cpp, %.o, $(CSRCS))

all: $(OBJ_DIR) lib $(APPNAME)

$(APPNAME): $(CXXOBJS) $(STATICLIB_DIR)/lib$(STATICLIB).a
	$(CC) $(LDFLAGS) -o $@ $(addprefix $(OBJ_DIR)/, $(CXXOBJS)) -L$(STATICLIB_DIR) -l$(STATICLIB) -lpthread -lstdc++

$(CXXOBJS): %.o : %.cpp
	$(CC) $(INCLUDE) -Wall $(CFLAGS) -c $^ -o $(OBJ_DIR)/$@

$(OBJ_DIR):
	if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi

$(STATICLIB_DIR)/lib$(STATICLIB).a :
	cd $(STATICLIB_DIR) && $(MAKE)

.PHONY: clean lib
clean:
	cd $(STATICLIB_DIR) && $(MAKE) clean
	rm -f $(APPNAME) $(CXXOBJS)
	rm -rf $(OBJ_DIR)

lib:
	cd $(STATICLIB_DIR) && $(MAKE)
