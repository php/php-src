--TEST--
Testing imagetruecolortopalette(): wrong types for first parameter
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php
$resource = tmpfile();
imagesetthickness($resource, 5);
?>
--EXPECTF--
Warning: imagesetthickness(): supplied resource is not a valid Image resource in %s on line %d
