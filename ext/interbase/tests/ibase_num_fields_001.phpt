--TEST--
ibase_num_fields(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

var_dump(ibase_num_fields(ibase_query('SELECT * FROM test1')));

var_dump(ibase_num_fields(1));
var_dump(ibase_num_fields());

?>
--EXPECTF--
int(2)

Warning: ibase_num_fields(): supplied argument is not a valid Firebird/InterBase result resource in %s on line %d
bool(false)

Warning: Wrong parameter count for ibase_num_fields() in %s on line %d
NULL
