--TEST--
Bug #46543 (ibase_trans() memory leaks when using wrong parameters)
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

@ibase_close();

ibase_trans(1);
ibase_trans();
ibase_trans('foo');
ibase_trans(fopen(__FILE__, 'r'));

$x = ibase_connect($test_base);
ibase_trans(1, 2, $x, $x, 3);

?>
--EXPECTF--
Warning: ibase_trans(): supplied resource is not a valid Firebird/InterBase link resource in %sbug46543.php on line %d

Warning: ibase_trans(): supplied resource is not a valid Firebird/InterBase link resource in %sbug46543.php on line %d

Warning: ibase_trans(): supplied resource is not a valid Firebird/InterBase link resource in %sbug46543.php on line %d

Warning: ibase_trans(): supplied resource is not a valid Firebird/InterBase link resource in %sbug46543.php on line %d
