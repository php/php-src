--TEST--
XORing strings
--FILE--
<?php

$s = "123";
$s1 = "test";
$s2 = "45345some";
$s3 = "f";

$s ^= 22;
var_dump($s);

$s1 ^= 11;
var_dump($s1);

$s2 ^= 33;
var_dump($s2);

$s3 ^= " ";
var_dump($s3);

echo "Done\n";
?>
--EXPECTF--
int(109)

Warning: A non-numeric value encountered in %s on line %d
int(11)

Notice: A non well formed numeric value encountered in %s on line %d
int(45312)
string(1) "F"
Done
