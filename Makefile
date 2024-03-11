owlcli:
	g++ ./tools/tracer.cpp ./tools/filebuffer.cpp ./ast/ast.cpp ./parser/parser.cpp ./interpreter/memobject.cpp ./cli/owl_cli.cpp ./lexer/lexer.cpp ./interpreter/callstack.cpp ./interpreter/memstore.cpp ./interpreter/interpreter.cpp -o owlcli

