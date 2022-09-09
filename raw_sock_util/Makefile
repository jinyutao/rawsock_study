
PROJNAME=rawsock

CSRCS = $(wildcard src/*.cpp)

INCLUDE += -I./

TARGET=lib$(PROJNAME).a

OBJ_DIR = obj
vpath %.o $(OBJ_DIR)

CXXOBJS = $(patsubst %.cpp, %.o, $(notdir $(CSRCS)))

all: $(TARGET)

$(TARGET): $(CXXOBJS)
	$(AR) -r $@ $(addprefix $(OBJ_DIR)/, $(CXXOBJS)) $(LDFLAGS)

$(CXXOBJS) : %.o : src/%.cpp
	if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi
	$(CC) $(INCLUDE) -Wall $(CFLAGS) -c $^ -o $(OBJ_DIR)/$@

.PHONY: clean
clean:
	rm -f $(TARGET) $(CXXOBJS)
	rm -rf $(OBJ_DIR)
