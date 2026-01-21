CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

MAIN = maximum_clique
GENERATOR = gen_tests
RUNNER = run_all

MAIN_SRC = maximum_clique.cpp
GEN_SRC = test_generator.cpp
RUN_SRC = run_all.cpp

GREEN = \033[0;32m
YELLOW = \033[1;33m
NC = \033[0m

.PHONY: all
all: $(MAIN) $(GENERATOR) $(RUNNER)
	@echo "$(GREEN)✓ Compilare completă! $(NC)"

$(MAIN): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(MAIN)

$(GENERATOR): $(GEN_SRC)
	$(CXX) $(CXXFLAGS) $(GEN_SRC) -o $(GENERATOR)

$(RUNNER): $(RUN_SRC)
	$(CXX) $(CXXFLAGS) $(RUN_SRC) -o $(RUNNER)

# Rulare completă
.PHONY: run
run: all
	@echo "$(YELLOW)1. Generare teste în folderul input/...$(NC)"
	@./$(GENERATOR)
	@echo "$(YELLOW)2. Rulare teste și salvare în output/...$(NC)"
	@./$(RUNNER)

.PHONY: clean
clean:
	@rm -f $(MAIN) $(GENERATOR) $(RUNNER)
	@rm -f clique.in clique.out
	@echo "$(GREEN)Executabile șterse$(NC)"

# Șterge tot, inclusiv folderele de date
.PHONY: clean-all
clean-all: clean
	@rm -rf input output
	@echo "$(GREEN)Folderele input/ și output/ au fost șterse$(NC)"