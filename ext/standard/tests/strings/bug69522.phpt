--TEST--
Bug #69522 (heap buffer overflow in unpack())
--FILE--
<?php
$a = pack("AAAAAAAAAAAA", 1,2,3,4,5,6,7,8,9,10,11,12);
$b = unpack('h2147483648', $a);
?>
--EXPECTF--
Warning: unpack(): Type h: integer overflow in %s on line %d
