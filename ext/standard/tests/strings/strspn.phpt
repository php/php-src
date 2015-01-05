--TEST--
Test strspn() behavior
--FILE--
<?php
$a = "22222222aaaa bbb1111 cccc";
$b = "1234";
var_dump($a);
var_dump($b);
var_dump(strspn($a,$b));
var_dump(strspn($a,$b,2));
var_dump(strspn($a,$b,2,3));
?>
--EXPECT--
string(25) "22222222aaaa bbb1111 cccc"
string(4) "1234"
int(8)
int(6)
int(3)
