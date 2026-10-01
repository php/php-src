--TEST--
pg_insert()/pg_update()/pg_delete(): the flags refused are the flags the message names
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = 'table_pg_dml_option_flags';

$conn = pg_connect($conn_str);
pg_query($conn, "CREATE TABLE {$table_name} (id INT, id2 INT)");

/* PGSQL_DML_ASYNC is not part of the accepted mask of these two */
try {
    pg_update($conn, $table_name, ['id2' => 2], ['id' => 1], PGSQL_DML_ASYNC | PGSQL_DML_STRING);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    pg_delete($conn, $table_name, ['id' => 1], PGSQL_DML_ASYNC | PGSQL_DML_STRING);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

/* 1 << 13 is not one of the flags at all */
try {
    pg_insert($conn, $table_name, ['id' => 1, 'id2' => 1], 1 << 13);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

/* but PGSQL_DML_ASYNC is accepted by pg_insert() and pg_select() */
var_dump(is_string(pg_insert($conn, $table_name, ['id' => 1, 'id2' => 1], PGSQL_DML_ASYNC | PGSQL_DML_STRING)));
var_dump(is_string(pg_select($conn, $table_name, ['id' => 1], PGSQL_DML_ASYNC | PGSQL_DML_STRING)));

/* every PGSQL_CONV_* flag the messages name is genuinely accepted */
var_dump(is_string(pg_insert($conn, $table_name, ['id' => 1, 'id2' => 1], PGSQL_CONV_IGNORE_DEFAULT | PGSQL_DML_STRING)));
var_dump(is_string(pg_update($conn, $table_name, ['id2' => 2], ['id' => 1], PGSQL_CONV_IGNORE_NOT_NULL | PGSQL_DML_STRING)));

/* PGSQL_DML_NO_CONV is accepted by pg_delete() and reaches its helper */
var_dump(is_string(pg_delete($conn, $table_name, ['id' => 1], PGSQL_DML_NO_CONV | PGSQL_DML_STRING)));

?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = 'table_pg_dml_option_flags';

$conn = pg_connect($conn_str);
pg_query($conn, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
pg_update(): Argument #5 ($flags) must be a valid bit mask of PGSQL_CONV_IGNORE_DEFAULT, PGSQL_CONV_FORCE_NULL, PGSQL_CONV_IGNORE_NOT_NULL, PGSQL_DML_NO_CONV, PGSQL_DML_ESCAPE, PGSQL_DML_EXEC, and PGSQL_DML_STRING
pg_delete(): Argument #4 ($flags) must be a valid bit mask of PGSQL_CONV_FORCE_NULL, PGSQL_DML_NO_CONV, PGSQL_DML_ESCAPE, PGSQL_DML_EXEC, and PGSQL_DML_STRING
pg_insert(): Argument #4 ($flags) must be a valid bit mask of PGSQL_CONV_IGNORE_DEFAULT, PGSQL_CONV_FORCE_NULL, PGSQL_CONV_IGNORE_NOT_NULL, PGSQL_DML_NO_CONV, PGSQL_DML_ESCAPE, PGSQL_DML_EXEC, PGSQL_DML_ASYNC, and PGSQL_DML_STRING
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
