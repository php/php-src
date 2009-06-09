--TEST--
Bug #46109 (MySQLi::init - Memory leaks)
--SKIPIF--
<?php 
require_once('skipif.inc'); 
?>
--FILE--
<?php
	include "connect.inc";

	$mysqli = new mysqli();
	$mysqli->init();
	$mysqli->init();
	echo "done";	
?>
--EXPECTF--
done
