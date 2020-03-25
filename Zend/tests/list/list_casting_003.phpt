--TEST--
"Cast during unpacking - nested unpacking" list()
--FILE--
<?php
$arr = [1, [2, 3], 4];

list((string) $a, list((bool) $b, (float) $c), $d) = $arr;
var_dump($a, $b, $c, $d);

[(string) $a, [(bool) $b, (float) $c], $d] = $arr;
var_dump($a, $b, $c, $d);
?>
--EXPECT--
string(1) "1"
bool(true)
float(3)
int(4)
string(1) "1"
bool(true)
float(3)
int(4)
