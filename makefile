CC=clang++
FLAGS=-std=$(STD)
APPNAME=swear_paper
DST_DIR=build

BOOST_DIR=/usr/local/include
LIBS=-lcurl -lopencv_highgui -lopencv_core -lopencv_imgproc -lboost_program_options
LIBS_LOC=-L/usr/local/lib -L/usr/lib

STD=c++14

INCLUDES = -I$(BOOST_DIR)

SRC_DIR = src

_OBJS = main.o image.o reddit_interface.o downloader.o earthporn.o

OBJ = $(patsubst %,$(DST_DIR)/%,$(_OBJS))

$(DST_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $< -o $@ $(FLAGS) $(INCLUDES)

swear_paper: $(OBJ)
	$(CC) -o $@ $^ $(LIBS_LOC) $(LIBS)

clean:
	rm $(DST_DIR)/*.o 