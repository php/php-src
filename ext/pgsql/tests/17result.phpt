--TEST--
PostgreSQL pg_fetch_*() functions
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'inc/config.inc';
$table_name = "table_17result";

$db = pg_connect($conn_str);
pg_query($db, "create table {$table_name} (num int, str text, bin bytea)");
pg_query($db, "insert into {$table_name} values(1, 'ABC', null)");
pg_query($db, "insert into {$table_name} values(1, 'ABC', null)");

$sql = "SELECT * FROM $table_name ORDER BY num";
$result = pg_query($db, $sql) or die('Cannot query db');
$rows = pg_num_rows($result);

var_dump(pg_result_seek($result, 1));
var_dump(pg_fetch_object($result));
var_dump(pg_fetch_array($result, 1));
var_dump(pg_fetch_row($result, 1));
var_dump(pg_fetch_assoc($result, 1));
var_dump(pg_result_seek($result, 0));

echo "Ok\n";
?>
--CLEAN--
<?php
include('inc/config.inc');
$table_name = "table_17result";

$db = pg_connect($conn_str);
pg_query($db, "drop table {$table_name}");
?>
--EXPECTF--
bool(true)
object(stdClass)#%d (3) {
  ["num"]=>
  string(1) "1"
  ["str"]=>
  string(3) "ABC"
  ["bin"]=>
  NULL
}
array(6) {
  [0]=>
  string(1) "1"
  ["num"]=>
  string(1) "1"
  [1]=>
  string(3) "ABC"
  ["str"]=>
  string(3) "ABC"
  [2]=>
  NULL
  ["bin"]=>
  NULL
}
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(3) "ABC"
  [2]=>
  NULL
}
array(3) {
  ["num"]=>
  string(1) "1"
  ["str"]=>
  string(3) "ABC"
  ["bin"]=>
  NULL
}
bool(true)
Ok
