--TEST--
Bug 73498 Incorrect DELIMITER syntax for pg_copy_to()
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include('inc/config.inc');

$db = pg_connect($conn_str);

$rows = pg_copy_to($db, "(select * from {$view_name})");

var_dump(gettype($rows));
var_dump(count($rows) > 0);

?>
--EXPECT--
string(5) "array"
bool(true)
