--TEST--
Divide 3 variables and print result
--FILE--
<?php $a=27; $b=3; $c=3; $d=$a/$b/$c; echo $d?>
--EXPECT--
3
