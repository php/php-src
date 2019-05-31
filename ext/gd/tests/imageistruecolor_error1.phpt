--TEST--
Testing imageistruecolor(): wrong parameters
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
    imageistruecolor($resource);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imageistruecolor(): supplied resource is not a valid Image resource
