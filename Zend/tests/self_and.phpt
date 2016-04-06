--TEST--
ANDing strings
--FILE--
<?php

$s = "123";
$s1 = "test";
$s2 = "45345some";

$s &= 22;
var_dump($s);

$s1 &= 11;
var_dump($s1);

$s2 &= 33;
var_dump($s2);

echo "Done\n";
?>
--EXPECTF--	
int(18)

Warning: A non-numeric value encountered in %s on line %d
int(0)

Notice: A non well formed numeric value encountered in %s on line %d
int(33)
Done
