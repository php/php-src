--TEST--
Bug #65119 (pg_copy_from() modifies input array variable)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php
include 'inc/config.inc';
$table_name = 'table_bug65119';

function test(Array $values, $conn_str) {
  global $table_name;
  $connection = pg_pconnect($conn_str, PGSQL_CONNECT_FORCE_NEW);
  pg_query($connection, "BEGIN");
  pg_query($connection, "CREATE TABLE {$table_name} (i INTEGER)");
  pg_copy_from($connection, $table_name, $values, "\t", "NULL");
  pg_query($connection, "ROLLBACK");
}

$values = Array(1,2,3);
var_dump($values);
test($values, $conn_str);
var_dump($values);
?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
