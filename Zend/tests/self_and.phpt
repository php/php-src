--TEST--
ANDing strings
--FILE--
<?php

$s = "123";
$s1 = "test";
$s2 = "45345some";
$s3 = str_repeat("f", 1);
$s4 = str_repeat("f", 2);

$s &= 22;
var_dump($s);

$s1 &= 11;
var_dump($s1);

$s2 &= 33;
var_dump($s2);

$s3 &= " ";
var_dump($s3);

$s4 &= "  ";
var_dump($s4);

echo "Done\n";
?>
--EXPECTF--
int(18)

Warning: A non-numeric value encountered in %s on line %d
int(0)

Notice: A non well formed numeric value encountered in %s on line %d
int(33)
string(1) " "
string(2) "  "
Done
