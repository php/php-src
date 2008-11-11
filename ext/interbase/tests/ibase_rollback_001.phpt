--TEST--
ibase_rollback(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

ibase_query('INSERT INTO test1 VALUES (100, 2)');
ibase_query('INSERT INTO test1 VALUES (100, 2)');
ibase_query('INSERT INTO test1 VALUES (100, 2)');

$rs = ibase_query('SELECT COUNT(*) FROM test1 WHERE i = 100');
var_dump(ibase_fetch_row($rs));

var_dump(ibase_rollback($x));

$rs = ibase_query('SELECT COUNT(*) FROM test1 WHERE i = 100');
var_dump(ibase_fetch_row($rs));

var_dump(ibase_rollback($x));
var_dump(ibase_rollback());

?>
--EXPECTF--
array(1) {
  [0]=>
  int(3)
}
bool(true)
array(1) {
  [0]=>
  int(0)
}
bool(true)

Warning: ibase_rollback(): invalid transaction handle (expecting explicit transaction start)  in %s on line %d
bool(false)
