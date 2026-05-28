# =============================
# Cross-platform Makefile
# Works on Linux, macOS, and Windows (MinGW / PowerShell / CMD)
# =============================

CC = gcc
CFLAGS = -Wall -Wextra -Werror -ansi -pedantic
INCLUDES = -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)
TARGET = assembler

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

.PHONY: clean

clean:
ifeq ($(OS),Windows_NT)
	-$(RM) obj\*.o 2>nul || true
	-$(RM) examples\valid\*.am 2>nul || true
	-$(RM) examples\valid\*.ent 2>nul || true
	-$(RM) examples\valid\*.ext 2>nul || true
	-$(RM) examples\valid\*.ob 2>nul || true
	-$(RM) examples\invalid\*.am 2>nul || true
	-$(RM) examples\invalid\*.ent 2>nul || true
	-$(RM) examples\invalid\*.ext 2>nul || true
	-$(RM) examples\invalid\*.ob 2>nul || true
	-$(RM) $(call FIXPATH,$(TARGET)).exe 2>nul || true
else
	-$(RM) obj/*.o $(TARGET)
	-$(RM) examples/valid/*.am examples/valid/*.ent examples/valid/*.ext examples/valid/*.ob
	-$(RM) examples/invalid/*.am examples/invalid/*.ent examples/invalid/*.ext examples/invalid/*.ob
endif