#!/bin/sh
SQLITE_VERSION=$(sqlite3 --version | awk '{print $1}')
wget -q "https://raw.githubusercontent.com/sqlite/sqlite/version-$SQLITE_VERSION/ext/fts5/fts5.h"
gcc -I. -g -fPIC -shared ftstri.c -o ftstri.so
rm fts5.h
