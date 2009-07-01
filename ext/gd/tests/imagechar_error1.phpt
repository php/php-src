--TEST--
Testing error on non-resource parameter 1 of imagechar() of GD library
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$result = imagechar('string', 1, 5, 5, 'C', 1);

?>
--EXPECTF--
Warning: imagechar() expects parameter 1 to be resource, %s given in %s on line %d
