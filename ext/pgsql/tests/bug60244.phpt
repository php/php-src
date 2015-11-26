--TEST--
Bug #60244 (pg_fetch_* functions do not validate that row param is >0)
--SKIPIF--
<?php
include("skipif.inc");
?>
--FILE--
<?php

include 'config.inc';

$db = pg_connect($conn_str);
$result = pg_query("select 'a' union select 'b'");

var_dump(pg_fetch_array($result, -1));
var_dump(pg_fetch_assoc($result, -1));
var_dump(pg_fetch_object($result, -1));
var_dump(pg_fetch_row($result, -1));

var_dump(pg_fetch_array($result, 0));
var_dump(pg_fetch_assoc($result, 0));
var_dump(pg_fetch_object($result, 0));
var_dump(pg_fetch_row($result, 0));

pg_close($db);

?>
--EXPECTF--
Warning: pg_fetch_array(): The row parameter must be greater or equal to zero in %sbug60244.php on line %d
bool(false)

Warning: pg_fetch_assoc(): The row parameter must be greater or equal to zero in %sbug60244.php on line %d
bool(false)

Warning: pg_fetch_object(): The row parameter must be greater or equal to zero in %sbug60244.php on line %d
bool(false)

Warning: pg_fetch_row(): The row parameter must be greater or equal to zero in %sbug60244.php on line %d
bool(false)
array(2) {
  [0]=>
  string(1) "a"
  ["?column?"]=>
  string(1) "a"
}
array(1) {
  ["?column?"]=>
  string(1) "a"
}
object(stdClass)#1 (1) {
  ["?column?"]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
