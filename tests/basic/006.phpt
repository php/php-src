--TEST--
Add 3 variables together and print result
--FILE--
<?php $a=1; $b=2; $c=3; $d=$a+$b+$c; echo $d?>
--EXPECT--
6
