owlcli:
	g++ --std=c++17 ./src/tools/tracer.cpp ./src/tools/filebuffer.cpp ./src/ast/ast.cpp ./src/parser/parser.cpp ./src/vm/runtime/memobject.cpp ./src/cli/cli.cpp ./src/owl_cli.cpp ./src/lexer/lexer.cpp ./src/vm/runtime/callstack.cpp ./src/vm/runtime/memstore.cpp ./src/vm/interpreter.cpp -o owl

owlrepl:
	g++ --std=c++17 ./src/tools/tracer.cpp ./src/tools/filebuffer.cpp ./src/ast/ast.cpp ./src/parser/parser.cpp ./src/vm/runtime/memobject.cpp ./src/repl/repl.cpp ./src/owl_repl.cpp ./src/lexer/lexer.cpp ./src/vm/runtime/callstack.cpp ./src/vm/runtime/memstore.cpp ./src/vm/interpreter.cpp -o owlrepl

owlshell:
	g++ --std=c++17 ./src/tools/tracer.cpp ./src/tools/filebuffer.cpp ./src/ast/ast.cpp ./src/parser/parser.cpp ./src/vm/runtime/memobject.cpp ./src/tokens/tokens.cpp ./src/shell/shell.cpp ./src/owl_shell.cpp ./src/lexer/lexer.cpp ./src/vm/runtime/callstack.cpp ./src/vm/runtime/memstore.cpp ./src/vm/interpreter.cpp -o owlsh

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
