--TEST--
Bug #28633 (sprintf incorrectly adding padding to floats)
--FILE--
<?php
	echo sprintf("%05.2f", 0.02) . "\n";
	echo sprintf("%05.2f", 2.02) . "\n";
?>
--EXPECT--
00.02
02.02
