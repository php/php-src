--TEST--
ibase_free_query(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

$q =ibase_prepare($x, 'SELECT 1 FROM test1 WHERE i = ?');
$q =ibase_prepare($x, 'SELECT 1 FROM test1 WHERE i = ?');
$q = ibase_prepare($x, 'SELECT 1 FROM test1 WHERE i = ?');

var_dump(ibase_free_query($q));
var_dump(ibase_free_query($q));
var_dump(ibase_free_query($x));

?>
--EXPECTF--
bool(true)

Warning: ibase_free_query(): 11 is not a valid Firebird/InterBase query resource in %s on line %d
bool(false)

Warning: ibase_free_query(): supplied resource is not a valid Firebird/InterBase query resource in %s on line %d
bool(false)
