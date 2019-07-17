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
try {
    imagesetthickness($resource, 5);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagesetthickness(): supplied resource is not a valid Image resource
