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
$image = imagecreatetruecolor(180, 30);
$resource = tmpfile();

imageistruecolor('string');
imageistruecolor($resource);
imageistruecolor(array());
?>
--EXPECTF--
Warning: imageistruecolor() expects parameter 1 to be resource, string given in %s on line %d

Warning: imageistruecolor(): supplied resource is not a valid Image resource in %s on line %d

Warning: imageistruecolor() expects parameter 1 to be resource, array given in %s on line %d