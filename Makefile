CPP_STANDARD = c++20
PYTHON = python3.12
PYDEV = $(shell $(PYTHON)-config --cflags --embed)

BINARIES = bin
SRC = src

WARNINGS = -Wall -Wstrict-prototypes

FLAGS = $(WARNINGS) -std=$(CPP_STANDARD)
LD_FLAGS = $(shell $(PYTHON)-config --embed --ldflags)

packages:
	$(PYTHON) -m pip install invoke
	$(PYTHON) -m pip install torch
	$(PYTHON) -m pip install torchvision

all:
	g++ $(FLAGS) $(SRC)/ironfly.cpp -I$(SRC) $(PYDEV) -o $(BINARIES)/ironfly $(LD_FLAGS)

