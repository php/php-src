--TEST--
PostgreSQL copy functions, part 2
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');
$table_name = 'table_06copy_2';

$db = pg_connect($conn_str);

pg_query($db, "CREATE TABLE {$table_name} (x int)");

pg_query($db, "COPY {$table_name} FROM STDIN");

pg_put_line($db, "1\n");
pg_put_line($db, "\\N\n");
pg_put_line($db, "\\.\n");
pg_end_copy($db);

var_dump(pg_fetch_all_columns(pg_query($db, "SELECT * FROM {$table_name} ORDER BY 1")));
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = 'table_06copy_2';

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS {$table_name}");
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  NULL
}
