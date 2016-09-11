CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CJsonTest

SRC = \
CJsonTest.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++11 \
-I$(INC_DIR) \
-I. \
-I../../CUtil/include \

LFLAGS = \
-L$(LIB_DIR) \
-L../../CJson/lib \
-L../../CStrUtil/lib \

clean:
	$(RM) -f *.o
	$(RM) -f CJsonTest

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CJsonTest: $(OBJS) $(LIB_DIR)/libCJson.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CJsonTest $(OBJS) $(LFLAGS) -lCJson -lCStrUtil
