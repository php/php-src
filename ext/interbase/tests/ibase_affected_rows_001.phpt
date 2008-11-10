--TEST--
ibase_affected_rows(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

ibase_query($x, 'INSERT INTO test1 VALUES (1, 100)');
ibase_query($x, 'INSERT INTO test1 VALUES (10000, 100)');

ibase_query($x, 'UPDATE test1 SET i = 10000');
var_dump(ibase_affected_rows($x));


ibase_query($x, 'UPDATE test1 SET i = 10000 WHERE i = 2.0');
var_dump(ibase_affected_rows($x));

ibase_query($x, 'UPDATE test1 SET i =');
var_dump(ibase_affected_rows($x));


?>
--EXPECTF--
int(3)
int(0)

Warning: ibase_query(): Dynamic SQL Error SQL error code = -104 Unexpected end of command %s on line %d
int(0)
