--TEST--
Bug #68044 Integer overflow in unserialize() (32-bits only)
--FILE--
<?php
	echo unserialize('C:3:"XYZ":18446744075857035259:{}');
?>
===DONE==
--EXPECTF--
Warning: Insufficient data for unserializing - %d required, 1 present in %s%ebug68044.php on line 2

Notice: unserialize(): Error at offset 32 of 33 bytes in %s%ebug68044.php on line 2
===DONE==
