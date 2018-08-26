CC=clang++
FLAGS=-std=$(STD) -Werror -Wall -Wextra -Wpedantic
DEBUG_FLAGS=-g -O0
RELEASE_FLAGS=-03

APPNAME=swear_paper
DST_DIR=build
RELEASE_DIR=release
DEBUG_DIR=debug
EXEC=swear_paper

BOOST_DIR=/usr/local/include
LIBS=-lcurl -lopencv_highgui -lopencv_core -lopencv_imgproc -lboost_program_options
LIBS_LOC=-L/usr/local/lib -L/usr/lib

STD=c++14

INCLUDES = -I$(BOOST_DIR)

SRC_DIR = src

_OBJS = main.o image.o downloader.o earthporn.o downloader_delegate.o image_size.o image_delegate.o image_location.o json_parse_delegate.o ansi_codes.o program_delegates.o

DEBUG_OBJS= $(patsubst %,$(DST_DIR)/$(DEBUG_DIR)/%,$(_OBJS))
RELEASE_OBJS=$(patsubst %, $(DST_DIR)/$(RELEASE_DIR)/%, $(_OBJS))

EXEC_DIR=./


debug: $(DEBUG_OBJS)
	$(CC) -o d$(EXEC) $^ $(FLAGS) $(DEBUG_FLAGS) $(LIBS_LOC) $(LIBS)

release: $(RELEASE_OBJS)
	$(CC) -o $(EXEC) $^ $(FLAGS) $(RELEASE_FLAGS) $(LIBS_LOC) $(LIBS)

$(DST_DIR)/$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(FLAGS) $(DBG_FLAGS) $(INCLUDES)

$(DST_DIR)/$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(FLAGS) $(RELEASE_FLAGS) $(INCLUDES)

clean:
	rm $(DST_DIR)/*/*.o $(EXEC) d$(EXEC)

format:
	clang-format -i -style=file src/*

cppcheck: debug
	cppcheck --enable=all $(SRC_DIR)

lldb: debug
	lldb ./d$(EXEC)

all: debug release