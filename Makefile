owlcli:
	g++ --std=c++17 ./src/tools/tracer.cpp ./src/tools/filebuffer.cpp ./src/ast/ast.cpp ./src/parser/parser.cpp ./src/interpreter/memobject.cpp ./src/cli/cli.cpp ./src/owl_cli.cpp ./src/lexer/lexer.cpp ./src/interpreter/callstack.cpp ./src/interpreter/memstore.cpp ./src/interpreter/interpreter.cpp -o owl

owlrepl:
	g++ --std=c++17 ./src/tools/tracer.cpp ./src/tools/filebuffer.cpp ./src/ast/ast.cpp ./src/parser/parser.cpp ./src/interpreter/memobject.cpp ./src/repl/repl.cpp ./src/owl_repl.cpp ./src/lexer/lexer.cpp ./src/interpreter/callstack.cpp ./src/interpreter/memstore.cpp ./src/interpreter/interpreter.cpp -o owlrepl

owlshell:
	g++ --std=c++17 ./src/tools/tracer.cpp ./src/tools/filebuffer.cpp ./src/ast/ast.cpp ./src/parser/parser.cpp ./src/interpreter/memobject.cpp ./src/tokens/tokens.cpp ./src/shell/shell.cpp ./src/owl_shell.cpp ./src/lexer/lexer.cpp ./src/interpreter/callstack.cpp ./src/interpreter/memstore.cpp ./src/interpreter/interpreter.cpp -o owlsh

all:
	make owlcli
	make owlrepl
	make owlshell

install:
	cp owl /usr/bin/owl

clean:
	rm ./owl
	rm ./owlrepl
	rm ./owlsh
