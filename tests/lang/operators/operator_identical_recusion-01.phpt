--TEST--
Test === operator : False recursion detection
--FILE--
<?php
$n = 0;
$a = [[$n]];
$b = [&$a];
var_dump($a === $b);
--EXPECT--
bool(false)
