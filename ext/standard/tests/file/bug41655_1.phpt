--TEST--
Bug #41655 (open_basedir bypass via glob())
--INI--
open_basedir=/tmp
--FILE--
<?php
	$a=glob("./*.jpeg");
?>
--EXPECTF--
Warning: glob(): open_basedir restriction in effect. File%s.) is not within the allowed path(s): (/tmp) in %s on line %d
