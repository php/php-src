--TEST--
Bug #41655 (open_basedir bypass via glob()) 2/2
--INI--
open_basedir=/
--FILE--
<?php
	$dir = dirname(__FILE__);
	$a=glob($dir . "/bug41655*.*");
	print_r($a);
?>
--EXPECTF--
Array
(
    [0] => %sbug41655_1.phpt
    [1] => %sbug41655_2.php
    [2] => %sbug41655_2.phpt
)
