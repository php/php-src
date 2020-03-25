--TEST--
"Cast during unpacking - nested associative unpacking" list()
--FILE--
<?php
$arr = ['a' => 1, 'b' => ['b1' => 2, 'b2' => 3], 'c' => 4];

list('a' => (string) $a, 'b' => list('b1' => (bool) $b, 'b2' => (float) $c), 'c' => $d) = $arr;
var_dump($a, $b, $c, $d);

['a' => (string) $a, 'b' => ['b1' => (bool) $b, 'b2' => (float) $c], 'c' => $d] = $arr;
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
