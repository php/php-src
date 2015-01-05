--TEST--
ibase_num_params(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

$rs = ibase_prepare('SELECT * FROM test1 WHERE 1 = ? AND 2 = ?');
var_dump(ibase_num_params($rs));

$rs = ibase_prepare('SELECT * FROM test1 WHERE 1 = ? AND 2 = ?');
var_dump(ibase_num_params());

$rs = ibase_prepare('SELECT * FROM test1 WHERE 1 = ? AND 2 = ? AND 3 = :x');
var_dump(ibase_num_params($rs));


?>
--EXPECTF--
int(2)

Warning: ibase_num_params() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: ibase_prepare(): Dynamic SQL Error SQL error code = -206 %s in %s on line %d

Warning: ibase_num_params() expects parameter 1 to be resource, boolean given in %s on line %d
NULL
