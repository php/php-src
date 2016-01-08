--TEST--
ibase_trans(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);
var_dump(ibase_trans($x));
var_dump(ibase_trans(1));
var_dump(ibase_close());
var_dump(ibase_close($x));

?>
--EXPECTF--
resource(%d) of type (Firebird/InterBase transaction)
resource(%d) of type (Firebird/InterBase transaction)
bool(true)
bool(true)
