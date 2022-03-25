# Source file
SRC = *.cpp
# Pakde.cpp,Pakde_OnInit.cpp,Pakde_OnRender.cpp,Pakde_OnCleanup.cpp,Pakde_OnEvent.cpp,Pakde_OnLoop.cpp

# Compiler to use
CC = g++

# Compiler flags (none currently)
CC_FLAGS = -g -Wall -Wextra -Wpedantic

# Linker flags, specifies libs to use
L_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf

# Out name
OUT = PAKDE_PARNO

# Compiles
all: $(SRC)
	$(CC) $(CC_FLAGS) $(SRC) $(L_FLAGS) -o $(OUT)