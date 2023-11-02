--TEST--
PostgreSQL copy functions, part 2
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);

pg_query($db, 'CREATE TABLE test_copy (x int)');

pg_query($db, 'COPY test_copy FROM STDIN');

pg_put_line($db, "1\n");
pg_put_line($db, "\\N\n");
pg_put_line($db, "\\.\n");
pg_end_copy($db);

var_dump(pg_fetch_all_columns(pg_query($db, 'SELECT * FROM test_copy ORDER BY 1')));
?>
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);

pg_query($db, 'DROP TABLE test_copy');
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  NULL
}
