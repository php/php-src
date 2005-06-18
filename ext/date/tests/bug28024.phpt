--TEST--
Bug #28024 (Changed behavior of strtotime())
--FILE--
<?php
	putenv("TZ=Europe/Berlin");
	echo strtotime("17:00 2004-01-01"), "\n";
	echo date("Y-m-d H:i:s T", strtotime("17:00 2004-01-01"));
?>
--EXPECT--
1072972800
2004-01-01 17:00:00 CET
