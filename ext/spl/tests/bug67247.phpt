--TEST--
Bug #67247 (spl_fixedarray_resize integer overflow)
--FILE--
<?php
$ar = new SplFixedArray(1);
echo "size: ".$ar->getSize()."\n";
$ar->setSize((PHP_INT_SIZE==8)?0x2000000000000001:0x40000001);
echo "size: ".$ar->getSize()."\n";
?>
--EXPECTF--
size: 1

Fatal error: Possible integer overflow in memory allocation (%d * %d + 0) in %s on line %d 
