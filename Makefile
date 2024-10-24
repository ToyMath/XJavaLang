CC = gcc
CXX = g++
LEX = flex
YACC = bison

CFLAGS = -c -g
CXXFLAGS = `llvm-config --cxxflags` -std=c++14 -c -g
LDFLAGS = `llvm-config --ldflags` -std=c++14 `llvm-config --libs`

all: xjava

xjava: lex.yy.o parser.tab.o ast.o XJavaLang.o
	$(CXX) -o xjava lex.yy.o parser.tab.o ast.o XJavaLang.o $(LDFLAGS)

lex.yy.o: lex.yy.c parser.tab.h
	$(CC) $(CFLAGS) lex.yy.c

parser.tab.o: parser.tab.c ast.h
	$(CC) $(CFLAGS) parser.tab.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) ast.c

XJavaLang.o: XJavaLang.cpp ast.h
	$(CXX) $(CXXFLAGS) XJavaLang.cpp

lex.yy.c: lexer.l
	$(LEX) lexer.l

parser.tab.c parser.tab.h: parser.y
	$(YACC) -d parser.y

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h *.o xjava output.ll output

run: xjava
	./xjava < Test.java > output.ll
	clang output.ll -o output
	./output

.PHONY: clean run
