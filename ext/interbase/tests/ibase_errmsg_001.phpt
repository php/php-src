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
Warning: ibase_query(): Dynamic SQL Error SQL error code = -104 Unexpected end of command  in %s on line %d
string(66) "Dynamic SQL Error SQL error code = -104 Unexpected end of command "
bool(false)
