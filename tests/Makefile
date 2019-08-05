
MAINNAME=main_test

#put source here
SRC_DIR=src

OBJ_DIR=obj

UNITTESTS_SYSTEM=Catch2

#SRCS=tests-main.cpp test-request.cpp
SRCS=test-request.cpp
LIBSRCS=$(addprefix $(SRC_DIR)/,$(SRCS)) 

OBJS=$(SRCS:.cpp=.o)
LIBOBJS =$(addprefix $(OBJ_DIR)/,$(OBJS)) 

CC=g++ 

CFLAGS= -Wall -Isrc -I$(UNITTESTS_SYSTEM)/single_include/catch2 -I../include

$(MAINNAME): $(OBJ_DIR) $(UNITTESTS_SYSTEM) $(LIBOBJS)
	$(CC) -Wall -o $@ $(CFLAGS) $(LIBOBJS)

$(UNITTESTS_SYSTEM):
	@echo "$(UNITTESTS_SYSTEM) folder is missing. Please run the download_tests_dependencies.sh script"
	@false

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CFLAGS) -c $^ -o $@

clean:
	rm -dr obj/*; rm $(MAINNAME); rm $(MAINNAME)

$(OBJ_DIR):
	mkdir -p $@

