--TEST--
Testing imagetruecolortopalette(): wrong parameters for parameter 1
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--SKIPIF--
<?php
	if (!extension_loaded("gd")) die("skip GD not present");
	if (!function_exists("imagecreatetruecolor")) die("skip GD Version not compatible");
?>
--FILE--
<?php
$resource = tmpfile();
try {
    imagetruecolortopalette($resource, true, 2);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagetruecolortopalette(): supplied resource is not a valid Image resource
