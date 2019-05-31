--TEST--
Testing imagecolorallocatealpha(): Wrong types for parameter 1
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
    imagecolorallocatealpha($resource, 255, 255, 255, 50);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagecolorallocatealpha(): supplied resource is not a valid Image resource
