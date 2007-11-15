--TEST--
Bug #41655 (open_basedir bypass via glob()) 2/2
--INI--
open_basedir=/
--FILE--
<?php
	$dir = dirname(__FILE__);
	$a=glob($dir . "/test.*");
	print_r($a);
?>
--EXPECTF--
Array
(
    [0] => %stest.csv
)
