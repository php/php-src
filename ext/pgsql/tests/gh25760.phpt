--TEST--
GH-25760: pg_fetch_all_columns() crashes with JIT when the result for an empty set is modified
--EXTENSIONS--
pgsql
opcache
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--FILE--
<?php
include("inc/config.inc");

function test(PgSql\Result $result) {
    $columns = pg_fetch_all_columns($result);
    $columns[] = 1;
    return $columns;
}

$conn = pg_connect($conn_str);
var_dump(test(pg_query($conn, "SELECT 1 WHERE false")));
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
