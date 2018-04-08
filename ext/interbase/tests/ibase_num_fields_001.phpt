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

Warning: ibase_num_fields() expects parameter 1 to be resource, int given in %s on line %d
NULL

Warning: ibase_num_fields() expects exactly 1 parameter, 0 given in %s on line %d
NULL
