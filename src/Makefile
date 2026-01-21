# Compiler și flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
DEBUG_FLAGS = -std=c++17 -g -Wall -Wextra -DDEBUG

# Fișiere executabile
MAIN = clique
GENERATOR = gen_tests

# Fișiere sursă
MAIN_SRC = maximum_clique.cpp
GEN_SRC = test_generator.cpp

# Culori pentru output
GREEN = \033[0;32m
YELLOW = \033[1;33m
NC = \033[0m

# Target implicit
.PHONY: all
all: $(MAIN) $(GENERATOR)
	@echo "$(GREEN)✓ Compilare completă! $(NC)"

# Compilare program principal
$(MAIN): $(MAIN_SRC)
	@echo "$(YELLOW)Compilare $(MAIN_SRC)...$(NC)"
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(MAIN)
	@echo "$(GREEN)✓ $(MAIN) compilat!$(NC)"

# Compilare generator teste
$(GENERATOR): $(GEN_SRC)
	@echo "$(YELLOW)Compilare $(GEN_SRC)...$(NC)"
	$(CXX) $(CXXFLAGS) $(GEN_SRC) -o $(GENERATOR)
	@echo "$(GREEN)✓ $(GENERATOR) compilat!$(NC)"

# Compilare cu debug
. PHONY: debug
debug:  CXXFLAGS = $(DEBUG_FLAGS)
debug: clean all
	@echo "$(GREEN)✓ Build debug gata!$(NC)"

# Curățare
.PHONY:  clean
clean:
	@echo "$(YELLOW)Curățare...$(NC)"
	@rm -f $(MAIN) $(GENERATOR)
	@echo "$(GREEN)✓ Executabile șterse$(NC)"

# Curățare completă (include fișiere de test)
.PHONY: clean-all
clean-all: clean
	@rm -f test*. in clique.in clique.out
	@echo "$(GREEN)✓ Toate fișierele șterse$(NC)"

# Ajutor
.PHONY: help
help:
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)   Makefile - Problema Clicii Maxime$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@echo ""
	@echo "$(YELLOW)Comenzi disponibile:$(NC)"
	@echo "  make              - Compilează toate programele"
	@echo "  make debug        - Compilează cu simboluri de debug"
	@echo "  make clean        - Șterge executabilele"
	@echo "  make clean-all    - Șterge tot (inclusiv fișiere test)"
	@echo "  make help         - Afișează acest mesaj"
	@echo ""

# Info despre versiunea compiler-ului
.PHONY:  info
info:
	@echo "$(GREEN)Informații sistem:$(NC)"
	@echo "Compiler: $(CXX)"
	@$(CXX) --version | head -n 1
	@echo "Flags: $(CXXFLAGS)"

.DEFAULT_GOAL := all
