--TEST--
mysqli get_client_info
--SKIPIF--
<?php	require_once('skipif.inc'); ?>
--FILE--
<?php
	$s = mysqli_get_client_info();
	echo gettype($s);
?>
--EXPECTF--
string
