#!/bin/sh

make clean

rm aclocal.m4
rm compile
rm depcomp
rm ar-lib
rm configure
rm install-sh
rm missing
rm config.log
rm config.status
rm -rf autom4te.cache

# 42sh
rm -rf .deps
rm Makefile
rm Makefile.in

# src
cd src
rm -rf .deps
rm Makefile
rm Makefile.in

# ast
cd ast
rm -rf .deps
rm Makefile
rm Makefile.in
cd ..

# builtins
cd builtins
rm -rf .deps
rm Makefile
rm Makefile.in
cd ..

# exec
cd exec
rm -rf .deps
rm Makefile
rm Makefile.in
cd ..

# exec_tree
cd exec_tree
rm -rf .deps
rm Makefile
rm Makefile.in
cd ..

# lexer
cd lexer
rm -rf .deps
rm Makefile
rm Makefile.in
cd ..

# parser
cd parser
rm -rf .deps
rm Makefile
rm Makefile.in
cd ..

# pretty_print
cd pretty_print
rm -rf .deps
rm Makefile
rm Makefile.in
cd ..

cd ..
