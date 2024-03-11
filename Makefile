owlcli:
	g++ --std=c++17 ./tools/tracer.cpp ./tools/filebuffer.cpp ./ast/ast.cpp ./parser/parser.cpp ./interpreter/memobject.cpp ./cli/owl_cli.cpp ./lexer/lexer.cpp ./interpreter/callstack.cpp ./interpreter/memstore.cpp ./interpreter/interpreter.cpp -o owlcli

owlrepl:
	g++ --std=c++17 ./tools/tracer.cpp ./tools/filebuffer.cpp ./ast/ast.cpp ./parser/parser.cpp ./interpreter/memobject.cpp ./cli/repl.cpp ./cli/owl_repl.cpp ./lexer/lexer.cpp ./interpreter/callstack.cpp ./interpreter/memstore.cpp ./interpreter/interpreter.cpp -o owlrepl

owlshell:
	g++ --std=c++17 ./tools/tracer.cpp ./tools/filebuffer.cpp ./ast/ast.cpp ./parser/parser.cpp ./interpreter/memobject.cpp ./tokens/tokens.cpp ./cli/shell.cpp ./cli/owl_shell.cpp ./lexer/lexer.cpp ./interpreter/callstack.cpp ./interpreter/memstore.cpp ./interpreter/interpreter.cpp -o owlsh

all:
	make owlcli
	make owlrepl
	make owlshell

clean:
	rm ./owlcli
	rm ./owlrepl
	rm ./owlsh
	rm .runlog.tree
