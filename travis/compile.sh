#!/bin/bash
./buildconf
./configure --with-pdo-mysql --with-mysql --with-sqlite --with-pdo-sqlite
make
