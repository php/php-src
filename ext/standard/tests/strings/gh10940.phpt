--TEST--
Test unpacking at the 32-bit integer limit
--FILE--
<?php
$a = pack("AAAAAAAAAAAA", 1,2,3,4,5,6,7,8,9,10,11,12);
unpack('h2147483647', $a);
?>
--EXPECTF--
Warning: unpack(): Type h: not enough input values, need 1073741824 values but only 12 were provided in %s on line %d
