CC       = g++
CFLAGS   = -std=c++17 -Wall -Wextra -Wshadow
RFLAGS   = -O3 -march=native -flto -DNDEBUG
DFLAGS   = -g
INC      = -I./libs/ -I./libs/libataxx/src/

LINKER   = g++
DLFLAGS  = -L./libs/libataxx/build/static/
LFLAGS   = -pthread -lboost_system -lataxx

TARGET   = cuteataxx
SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SOURCES:.cpp=.o))

$(BINDIR)/$(TARGET): $(BINDIR) $(OBJDIR) $(OBJECTS)
	@$(LINKER) -o $@ $(OBJECTS) $(DLFLAGS) $(LFLAGS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@

release:
	$(MAKE) CFLAGS="$(CFLAGS) $(RFLAGS)"

debug:
	$(MAKE) CFLAGS="$(CFLAGS) $(DFLAGS)" TARGET="$(TARGET)-debug"

bin:
	mkdir -p $(BINDIR)

obj:
	mkdir -p $(OBJDIR)

clean:
	rm -r $(OBJDIR) $(BINDIR)/$(TARGET) $(BINDIR)/$(TARGET)-debug

.PHONY: clean
