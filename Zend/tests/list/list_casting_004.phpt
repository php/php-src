--TEST--
"Cast during unpacking - associative unpacking" list()
--FILE--
<?php
$arr = ['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4];

list('a' => (string) $a, 'b' => (bool) $b, 'c' => (float) $c, 'd' => $d) = $arr;
var_dump($a, $b, $c, $d);

['a' => (string) $a, 'b' => (bool) $b, 'c' => (float) $c, 'd' => $d] = $arr;
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
