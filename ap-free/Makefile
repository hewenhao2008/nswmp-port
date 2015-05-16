TARGET_NAME = ap
TARGET_D_LIB := -lpthread -luuid
CFLAGS = -g -O2 -Winline -D __DEBUG__
SRC_BASE = .
INSTALL_PATH = 
CC = gcc
LIB_NAME = lib${TARGET_NAME}.a
LIBSO_NAME	= lib${TARGET_NAME}.so
OBJ_DIR = obj



FILES = $(wildcard $(SRC_BASE)/*.c)
TMP_C = $(notdir $(FILES))
OBJS = $(TMP_C:%.c=$(OBJ_DIR)/%.o)
SUBDIRS = $(patsubst %/,%,$(dir $(wildcard */Makefile)))

.PHONY: make_target
make_target: $(OBJ_DIR) $(OBJS) $(SUBDIRS)
	$(CC) $(CFLAGS) $(OBJS)  ${TARGET_D_LIB} -o $(TARGET_NAME)
	$(CC) -fPIC -shared  $(OBJS) ${TARGET_D_LIB} -o $(LIBSO_NAME)
	

.PHONY: all
all: make_target

.PHONY: install
install:
	cp $(TARGET_NAME) $(INSTALL_PATH)	

$(OBJ_DIR):
	-mkdir $(OBJ_DIR)
	echo $(TMP_C)

$(OBJ_DIR)/%.o:	$(SRC_BASE)/%.c
	$(CC) -c -fPIC $(CFLAGS) $(CFLAGS_$(<:.c=.o)) $(IFLAGS) $(IFLAGS_$(<:.c=.o)) -o $@ $<

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(TARGET_NAME)
	rm -rf $(LIBSO_NAME)
