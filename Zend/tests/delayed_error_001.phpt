--TEST--
Delayed error 001
--INI--
opcache.jit=0
--FILE--
<?php
$array[0][1] .= 'foo';
$array[2][3]++;
$array[3][4]--;
++$array[5][6];
--$array[7][8];
$array[9][10] += 42;
?>
--EXPECTF--
Warning: Undefined variable $array in %s on line %d

Warning: Undefined array key 0 in %s on line %d

Warning: Undefined array key 1 in %s on line %d

Warning: Undefined array key 2 in %s on line %d

Warning: Undefined array key 3 in %s on line %d

Warning: Decrement on type null has no effect, this will change in the next major version of PHP in %s on line %d

Warning: Undefined array key 3 in %s on line %d

Warning: Undefined array key 4 in %s on line %d

Warning: Undefined array key 5 in %s on line %d

Warning: Undefined array key 6 in %s on line %d

Warning: Decrement on type null has no effect, this will change in the next major version of PHP in %s on line %d

Warning: Undefined array key 7 in %s on line %d

Warning: Undefined array key 8 in %s on line %d

Warning: Undefined array key 9 in %s on line %d

Warning: Undefined array key 10 in %s on line %d
