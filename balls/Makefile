main: main.cpp
	clang-format -i -style="{BasedOnStyle: LLVM, IndentWidth: 4, TabWidth: 4, UseTab: Never}" main.cpp
	g++ main.cpp -o balls -I/usr/local/include -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system -O3 -ffast-math
