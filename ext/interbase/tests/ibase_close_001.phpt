--TEST--
ibase_close(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);
var_dump(ibase_close($x));
var_dump(ibase_close($x));
var_dump(ibase_close());
var_dump(ibase_close('foo'));

?>
--EXPECTF--
bool(true)
bool(true)

Warning: ibase_close(): supplied resource is not a valid Firebird/InterBase link resource in %s on line %d
bool(false)

Warning: ibase_close() expects parameter 1 to be resource,%string given in %s on line %d
NULL
