--TEST--
ibase_errmsg(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

ibase_query('SELECT Foobar');
var_dump(ibase_errmsg());

ibase_close($x);
var_dump(ibase_errmsg());

?>
--EXPECTF--
Warning: ibase_query(): Dynamic SQL Error SQL error code = -104 %s on line %d
string(%d) "Dynamic SQL Error SQL error code = -104 %s"
bool(false)
