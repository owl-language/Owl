owlcli:
	g++ --std=c++17 -c ./src/tools/tracer.cpp
	g++ --std=c++17 -c ./src/tools/filebuffer.cpp 
	g++ --std=c++17 -c ./src/ast/ast.cpp 
	g++ --std=c++17 -c ./src/parser/parser.cpp 
	g++ --std=c++17 -c ./src/vm/runtime/memobject.cpp 
	g++ --std=c++17 -c ./src/cli/cli.cpp 
	g++ --std=c++17 -c ./src/repl/repl.cpp 
	g++ --std=c++17 -c ./src/lexer/lexer.cpp 
	g++ --std=c++17 -c ./src/vm/runtime/callstack.cpp 
	g++ --std=c++17 -c ./src/vm/runtime/memstore.cpp 
	g++ --std=c++17 -c ./src/vm/interpreter.cpp 
	g++ --std=c++17 -c ./src/owl_cli.cpp 
	g++ --std=c++17 -o owl *.o
	mv ./*.o build/

owlshell:
	g++ --std=c++17 ./src/tools/tracer.cpp ./src/tools/filebuffer.cpp ./src/ast/ast.cpp ./src/parser/parser.cpp ./src/vm/runtime/memobject.cpp ./src/tokens/tokens.cpp ./src/shell/shell.cpp ./src/owl_shell.cpp ./src/lexer/lexer.cpp ./src/vm/runtime/callstack.cpp ./src/vm/runtime/memstore.cpp ./src/vm/interpreter.cpp -o owlsh

all:
	make owlcli
	make owlshell

install:
	cp owl /usr/bin/owl

clean:
	rm ./owl
	rm ./owlsh
	rm ./build/*.o