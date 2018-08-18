CC=clang++
FLAGS=-std=$(STD) -Werror -Wall -Wextra -Wpedantic
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

$(DST_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(FLAGS) $(INCLUDES)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LIBS_LOC) $(LIBS)

clean:
	rm $(DST_DIR)/*.o $(EXEC)

format:
	clang-format -i -style=file src/*