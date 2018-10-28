--TEST--
Invalid format type validation
--FILE--
<?php
	var_dump(unpack("-2222", 1));
	echo "Done\n";
?>
--EXPECTF--
Warning: unpack(): Invalid format type - in %sunpack.php on line %d
bool(false)
Done
