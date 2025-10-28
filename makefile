# =============================
# Cross-platform Makefile
# Works on Linux, macOS, and Windows (MinGW / PowerShell / CMD)
# =============================

CC = gcc
CFLAGS = -Wall -ansi -pedantic
INCLUDES = -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)
TARGET = bin/assembler

# Detect OS type
ifeq ($(OS),Windows_NT)
	RM = del /q
	MKDIR = if not exist obj mkdir obj
	FIXPATH = $(subst /,\\,$1)
else
	RM = rm -f
	MKDIR = mkdir -p obj
	FIXPATH = $1
endif

# =============================
# Rules
# =============================

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJ)

obj/%.o: src/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean run

run: $(TARGET)
	./$(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	-$(RM) obj\*.o 2>nul || true
	-$(RM) tests\*.am 2>nul || true
	-$(RM) tests\*.ent 2>nul || true
	-$(RM) tests\*.ext 2>nul || true
	-$(RM) tests\*.ob 2>nul || true
	-$(RM) $(call FIXPATH,$(TARGET)).exe 2>nul || true
else
	-$(RM) obj/*.o $(TARGET)
endif
