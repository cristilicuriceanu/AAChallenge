CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic
TARGET = kclique

. PHONY: all clean run test help

all: $(TARGET)

$(TARGET): kclique.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) kclique.cpp

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@echo "=== Test cu graf implicit ==="
	./$(TARGET)
	@echo ""
	@if [ -f datasets/easy_small.txt ]; then \
		echo "=== Test cu dataset easy_small ==="; \
		./$(TARGET) datasets/easy_small.txt; \
	fi

benchmark: $(TARGET)
	@echo "Rulare benchmark pe toate datasetele..."
	@for file in datasets/*.txt; do \
		if [ -f "$$file" ]; then \
			echo ""; \
			echo "Testing: $$file"; \
			./$(TARGET) "$$file"; \
		fi; \
	done

clean:
	rm -f $(TARGET)

help:
	@echo "Comenzi disponibile:"
	@echo "  make          - Compilează programul"
	@echo "  make run      - Rulează cu graf de test"
	@echo "  make test     - Rulează teste"
	@echo "  make benchmark - Rulează pe toate datasetele"
	@echo "  make clean    - Șterge fișierele compilate"
	@echo ""
	@echo "Utilizare:"
	@echo "  ./k_clique_solver                    - Graf de test"
	@echo "  ./k_clique_solver <fisier> [k]       - Încarcă din fișier"
