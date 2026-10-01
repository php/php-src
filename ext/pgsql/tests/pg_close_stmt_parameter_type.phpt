--TEST--
pg_close_stmt(): the connection parameter is typed like every other pgsql function
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
if (!function_exists('pg_close_stmt')) die('skip pg_close_stmt() requires libpq >= 17');
?>
--FILE--
<?php

foreach (['pg_close_stmt', 'pg_connect_poll'] as $function) {
    $parameter = (new ReflectionFunction($function))->getParameters()[0];
    printf("%-16s %s\n", $function, $parameter->getType());
}

var_dump((new ReflectionClass(PgSql\Connection::class))->getName());
?>
--EXPECT--
pg_close_stmt    PgSql\Connection
pg_connect_poll  PgSql\Connection
string(16) "PgSql\Connection"
