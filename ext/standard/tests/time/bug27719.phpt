--TEST--
Bug #27719: mktime returns incorrect timestamp for dst days
--FILE--
<?php
	putenv("TZ=EST");

	echo mktime(0, 0, 0, 4, 4, 2004, 0) . "\n";
	echo mktime(0, 0, 0, 4, 4, 2004, 1) . "\n";
	echo mktime(0, 0, 0, 4, 4, 2004, -1) . "\n";
?>
--EXPECT--
1081054800
1081051200
1081054800
