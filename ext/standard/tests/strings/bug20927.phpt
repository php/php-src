--TEST--
Segfault on wordwrap statement bug #20927
--SKIPIF--
--FILE--
<?php
$x = "ADV:CLAIM YOUR FORTUNE NOW !!MAKE";
$x .= " xxxxxxxxxxHUNDREDS OF THOUSANDSxxxxxxxxxxxx";
$b = "CANITBREAKFOO\n";
$x = wordwrap($x, 20, $b, 1);
$x = wordwrap($x, 20, $b, 1);
print "$x\n";
?>
--EXPECT--
ADV:CLAIM YOURCANITBREAKFOO
FORTUNE NOW !!MAKECANITBREAKFOO
xxxxxxxxxxHUNDREDSCANITBREAKFOO
OFCANITBREAKFOO
THOUSANDSxxxxxxxxxxxCANITBREAKFOO
x
