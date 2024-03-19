owlcli:
	g++ --std=c++17 -c ./src/tools/tracer.cpp
	g++ --std=c++17 -c ./src/tools/filebuffer.cpp 
	g++ --std=c++17 -c ./src/ast/ast.cpp 
	g++ --std=c++17 -c ./src/parser/parser.cpp 
	g++ --std=c++17 -c ./src/cli/cli.cpp 
	g++ --std=c++17 -c ./src/repl/repl.cpp 
	g++ --std=c++17 -c ./src/lexer/lexer.cpp 
	g++ --std=c++17 -c ./src/interpreter/runtime/record.cpp
	g++ --std=c++17 -c ./src/interpreter/runtime/callstack.cpp 
	g++ --std=c++17 -c ./src/interpreter/runtime/memobject.cpp 
	g++ --std=c++17 -c ./src/interpreter/runtime/memstore.cpp 
	g++ --std=c++17 -c ./src/interpreter/eval.cpp
	g++ --std=c++17 -c ./src/interpreter/interpreter.cpp 
	g++ --std=c++17 -c ./src/owl_cli.cpp 
	g++ --std=c++17 -g -o owl *.o
	mv ./*.o build/

debug:
	g++ --std=c++17 -g -c ./src/tools/tracer.cpp
	g++ --std=c++17 -g -c ./src/tools/filebuffer.cpp 
	g++ --std=c++17 -g -c ./src/ast/ast.cpp 
	g++ --std=c++17 -g -c ./src/parser/parser.cpp 
	g++ --std=c++17 -g -c ./src/cli/cli.cpp 
	g++ --std=c++17 -g -c ./src/repl/repl.cpp 
	g++ --std=c++17 -g -c ./src/lexer/lexer.cpp 
	g++ --std=c++17 -g -c ./src/interpreter/runtime/record.cpp
	g++ --std=c++17 -g -c ./src/interpreter/runtime/callstack.cpp 
	g++ --std=c++17 -g -c ./src/interpreter/runtime/memobject.cpp 
	g++ --std=c++17 -g -c ./src/interpreter/runtime/memstore.cpp 
	g++ --std=c++17 -g -c ./src/interpreter/eval.cpp
	g++ --std=c++17 -g -c ./src/interpreter/interpreter.cpp 
	g++ --std=c++17 -g -c ./src/owl_cli.cpp 
	g++ --std=c++17 -g -o owl *.o
	mv ./*.o build/

all:
	make owlcli

install:
	cp owl /usr/bin/owl

clean:
	rm ./owl
	rm ./build/*.o