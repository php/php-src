--TEST--
Bug #76548 pg_fetch_result did not fetch the next row
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include('inc/config.inc');

$conn = pg_connect($conn_str);

$result = pg_query($conn, 'SELECT v FROM (VALUES (1), (2), (3)) AS t(v)');

while ($value = pg_fetch_result($result, 0)) {
  var_dump($value); // should be 1, 2 then 3.
}

?>
--EXPECT--
string(1) "1"
string(1) "2"
string(1) "3"
