--TEST--
Bug #41655 (open_basedir bypass via glob()) 1/2
--INI--
open_basedir=/tmp
--FILE--
<?php
	$a=glob("./*.jpeg");

echo "Done\n";
?>
--EXPECT--
Done