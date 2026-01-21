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

# Fișiere de intrare/ieșire
INPUT = clique.in
OUTPUT = clique.out

# Culori pentru output
GREEN = \033[0;32m
YELLOW = \033[1;33m
RED = \033[0;31m
NC = \033[0m # No Color

# Target implicit
.PHONY: all
all: $(MAIN) $(GENERATOR)
	@echo "$(GREEN)✓ Compilare completă! $(NC)"

# Compilare program principal
$(MAIN): $(MAIN_SRC)
	@echo "$(YELLOW)Compilare $(MAIN_SRC)...$(NC)"
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(MAIN)
	@echo "$(GREEN)✓ $(MAIN) gata!$(NC)"

# Compilare generator teste
$(GENERATOR): $(GEN_SRC)
	@echo "$(YELLOW)Compilare $(GEN_SRC)...$(NC)"
	$(CXX) $(CXXFLAGS) $(GEN_SRC) -o $(GENERATOR)
	@echo "$(GREEN)✓ $(GENERATOR) gata!$(NC)"

# Compilare cu debug
. PHONY: debug
debug:  CXXFLAGS = $(DEBUG_FLAGS)
debug: clean $(MAIN)
	@echo "$(GREEN)✓ Build debug gata!$(NC)"

# Rulează programul principal
.PHONY: run
run: $(MAIN)
	@if [ !  -f $(INPUT) ]; then \
		echo "$(RED)✗ Fișierul $(INPUT) nu există!$(NC)"; \
		echo "$(YELLOW)Rulează 'make generate' pentru a crea teste. $(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)Rulare $(MAIN)...$(NC)"
	@echo "$(YELLOW)============================================$(NC)"
	./$(MAIN)
	@echo "$(YELLOW)============================================$(NC)"

# Generează toate testele
.PHONY: generate
generate: $(GENERATOR)
	@echo "$(YELLOW)Generare teste...$(NC)"
	./$(GENERATOR)
	@echo "$(GREEN)✓ Teste generate!$(NC)"

# Generează și selectează un test specific
.PHONY: test1
test1: generate
	@cp test1_small.in $(INPUT)
	@echo "$(GREEN)✓ Test 1 (small) pregătit în $(INPUT)$(NC)"

.PHONY: test2
test2: generate
	@cp test2_medium.in $(INPUT)
	@echo "$(GREEN)✓ Test 2 (medium) pregătit în $(INPUT)$(NC)"

.PHONY: test3
test3: generate
	@cp test3_clique.in $(INPUT)
	@echo "$(GREEN)✓ Test 3 (with clique) pregătit în $(INPUT)$(NC)"

.PHONY: test4
test4: generate
	@cp test4_sparse.in $(INPUT)
	@echo "$(GREEN)✓ Test 4 (sparse) pregătit în $(INPUT)$(NC)"

.PHONY: test5
test5: generate
	@cp test5_dense.in $(INPUT)
	@echo "$(GREEN)✓ Test 5 (dense) pregătit în $(INPUT)$(NC)"

# Rulează un test specific
.PHONY:  run-test1
run-test1: test1 run

.PHONY: run-test2
run-test2: test2 run

.PHONY: run-test3
run-test3: test3 run

.PHONY: run-test4
run-test4: test4 run

.PHONY: run-test5
run-test5: test5 run

# Rulează toate testele
.PHONY:  run-all
run-all: $(MAIN) generate
	@echo "$(YELLOW)========================================$(NC)"
	@echo "$(YELLOW)        RULARE TOATE TESTELE$(NC)"
	@echo "$(YELLOW)========================================$(NC)"
	@for i in 1 2 3 4 5; do \
		echo "\n$(GREEN)>>> Test $$i <<<$(NC)"; \
		cp test$$i*. in $(INPUT) 2>/dev/null; \
		./$(MAIN); \
		echo "$(YELLOW)----------------------------------------$(NC)"; \
	done

# Verifică output-ul
.PHONY: check
check: 
	@if [ -f $(OUTPUT) ]; then \
		echo "$(GREEN)Conținut $(OUTPUT):$(NC)"; \
		cat $(OUTPUT); \
	else \
		echo "$(RED)✗ Fișierul $(OUTPUT) nu există!$(NC)"; \
	fi

# Curăță fișierele generate
.PHONY: clean
clean:
	@echo "$(YELLOW)Curățare fișiere...$(NC)"
	rm -f $(MAIN) $(GENERATOR)
	@echo "$(GREEN)✓ Executabile șterse$(NC)"

# Curăță tot (inclusiv teste și output)
.PHONY: clean-all
clean-all: clean
	rm -f test*.in $(INPUT) $(OUTPUT)
	@echo "$(GREEN)✓ Toate fișierele șterse$(NC)"

# Afișează informații despre Makefile
.PHONY:  help
help:
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)   Makefile - Problema Clicii Maxime$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@echo ""
	@echo "$(YELLOW)Target-uri disponibile:$(NC)"
	@echo ""
	@echo "  $(GREEN)make$(NC) sau $(GREEN)make all$(NC)        - Compilează toate programele"
	@echo "  $(GREEN)make debug$(NC)              - Compilează cu simboluri de debug"
	@echo "  $(GREEN)make run$(NC)                - Rulează programul principal"
	@echo "  $(GREEN)make generate$(NC)           - Generează toate testele"
	@echo ""
	@echo "  $(GREEN)make test1$(NC)              - Pregătește test 1 (small)"
	@echo "  $(GREEN)make test2$(NC)              - Pregătește test 2 (medium)"
	@echo "  $(GREEN)make test3$(NC)              - Pregătește test 3 (with clique)"
	@echo "  $(GREEN)make test4$(NC)              - Pregătește test 4 (sparse)"
	@echo "  $(GREEN)make test5$(NC)              - Pregătește test 5 (dense)"
	@echo ""
	@echo "  $(GREEN)make run-test1$(NC)          - Generează și rulează test 1"
	@echo "  $(GREEN)make run-test2$(NC)          - Generează și rulează test 2"
	@echo "  $(GREEN)make run-test3$(NC)          - Generează și rulează test 3"
	@echo "  $(GREEN)make run-test4$(NC)          - Generează și rulează test 4"
	@echo "  $(GREEN)make run-test5$(NC)          - Generează și rulează test 5"
	@echo "  $(GREEN)make run-all$(NC)            - Rulează toate testele"
	@echo ""
	@echo "  $(GREEN)make check$(NC)              - Afișează conținutul fișierului de output"
	@echo "  $(GREEN)make clean$(NC)              - Șterge executabilele"
	@echo "  $(GREEN)make clean-all$(NC)          - Șterge tot (inclusiv teste)"
	@echo "  $(GREEN)make help$(NC)               - Afișează acest mesaj"
	@echo ""
	@echo "$(YELLOW)Exemple de utilizare:$(NC)"
	@echo "  $(GREEN)make && make run-test1$(NC)  - Compilează și rulează test 1"
	@echo "  $(GREEN)make run-all$(NC)            - Rulează toate testele"
	@echo ""

# Previne ștergerea fișierelor intermediare
.PRECIOUS: test%. in

# Validează că toate fișierele sursă există
.PHONY: validate
validate:
	@echo "$(YELLOW)Validare fișiere...$(NC)"
	@if [ ! -f $(MAIN_SRC) ]; then \
		echo "$(RED)✗ $(MAIN_SRC) lipsește!$(NC)"; \
		exit 1; \
	fi
	@if [ !  -f $(GEN_SRC) ]; then \
		echo "$(RED)✗ $(GEN_SRC) lipsește!$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)✓ Toate fișierele sursă există$(NC)"

# Info despre versiunea compiler-ului
.PHONY: info
info:
	@echo "$(GREEN)Informații sistem:$(NC)"
	@echo "Compiler: $(CXX)"
	@$(CXX) --version | head -n 1
	@echo "Flags: $(CXXFLAGS)"
