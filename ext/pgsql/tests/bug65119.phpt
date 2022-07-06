--TEST--
Bug #65119 (pg_copy_from() modifies input array variable)
--SKIPIF--
<?php
include("skipif.inc");
?>
--FILE--
<?php
include 'config.inc';

function test(Array $values, $conn_str) {
  $connection = pg_pconnect($conn_str, PGSQL_CONNECT_FORCE_NEW);
  pg_query("begin");
  pg_query("CREATE TABLE bug65119 (i INTEGER)");
  pg_copy_from($connection, "bug65119", $values, "\t", "NULL");
  pg_query("rollback");
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
