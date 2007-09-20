--TEST--
Bug #41655 (open_basedir bypass via glob())
--INI--
open_basedir=/tmp
--FILE--
<?php
	$a=glob("./*.jpeg");

echo "Done\n";
?>
--EXPECT--
Done