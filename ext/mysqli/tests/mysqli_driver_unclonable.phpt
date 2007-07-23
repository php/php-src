--TEST--
Trying to clone mysqli_driver object
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	$driver = new mysqli_driver;
	$driver_clone = clone $driver;
	print "done!";
?>
--EXPECTF--
Fatal error: Trying to clone an uncloneable object of class mysqli_driver in %s on line %d