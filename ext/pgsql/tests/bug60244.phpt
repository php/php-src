--TEST--
Bug #60244 (pg_fetch_* functions do not validate that row param is >0)
--EXTENSIONS--
pgsql
--SKIPIF--
<?php
include("inc/skipif.inc");
?>
--FILE--
<?php

include 'inc/config.inc';

$db = pg_connect($conn_str);
$result = pg_query($db, "select 'a' union select 'b'");

try {
    var_dump(pg_fetch_array($result, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(pg_fetch_assoc($result, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(pg_fetch_object($result, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(pg_fetch_row($result, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(pg_fetch_array($result, 0));
var_dump(pg_fetch_assoc($result, 0));
var_dump(pg_fetch_object($result, 0));
var_dump(pg_fetch_row($result, 0));

pg_close($db);

?>
--EXPECTF--
pg_fetch_array(): Argument #2 ($row) must be greater than or equal to 0
pg_fetch_assoc(): Argument #2 ($row) must be greater than or equal to 0
pg_fetch_object(): Argument #2 ($row) must be greater than or equal to 0
pg_fetch_row(): Argument #2 ($row) must be greater than or equal to 0
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
object(stdClass)#%d (1) {
  ["?column?"]=>
  string(1) "a"
}
array(1) {
  [0]=>
  string(1) "a"
}
