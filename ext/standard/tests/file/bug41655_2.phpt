--TEST--
Bug #41655: open_basedir bypass via glob()
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
    [%d] => %sbug41655_1.phpt
    [%d] => %sbug41655_2.phpt
)
