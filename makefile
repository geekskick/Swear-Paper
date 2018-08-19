CC=clang++
FLAGS=-std=$(STD) -Werror -Wall -Wextra -Wpedantic
DBG_FLAGS=-g -O0
APPNAME=swear_paper
DST_DIR=build
EXEC=swear_paper

BOOST_DIR=/usr/local/include
LIBS=-lcurl -lopencv_highgui -lopencv_core -lopencv_imgproc -lboost_program_options
LIBS_LOC=-L/usr/local/lib -L/usr/lib

STD=c++14

INCLUDES = -I$(BOOST_DIR)

SRC_DIR = src

_OBJS = main.o image.o downloader.o earthporn.o downloader_delegate.o image_size.o image_delegate.o image_location.o json_parse_delegate.o ansi_codes.o program_delegates.o

OBJ = $(patsubst %,$(DST_DIR)/%,$(_OBJS))

EXEC_DIR=./

debug: $(OBJ)
	$(CC) -o d$(EXEC) $^ $(DBG_FLAGS) $(FLAGS) $(LIBS_LOC) $(LIBS)

release: $(OBJ)
	$(CC) -o $(EXEC) $^ $(FLAGS) $(LIBS_LOC) $(LIBS)

$(DST_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(FLAGS) $(DBG_FLAGS) $(INCLUDES)

clean:
	rm $(DST_DIR)/*.o $(EXEC) d$(EXEC)

format:
	clang-format -i -style=file src/*

cppcheck: debug
	cppcheck --enable=all $(SRC_DIR)

lldb: debug
	lldb ./d$(EXEC)

all: debug release