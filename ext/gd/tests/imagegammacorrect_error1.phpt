--TEST--
Testing error with non-resource paramenter of imagegammacorrect() of GD library, 
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php 
	if (!extension_loaded("gd")) die("skip GD not present");
?>
--FILE--
<?php

$gamma = imagegammacorrect('string', 1, 5);

?>
--EXPECTF--
Warning: imagegammacorrect(): supplied argument is not a valid Image resource in %s on line %d
