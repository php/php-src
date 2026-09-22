--TEST--
Sorting single-element arrays does not invoke the comparison function
--FILE--
<?php

$calls = 0;
$compare = static function ($a, $b) use (&$calls) {
    $calls++;
    return $a <=> $b;
};

$array = [42];
// Keep the array packed and without holes, but leave the next free index at 11.
$array[10] = 99;
unset($array[10]);
next($array);
var_dump(usort($array, $compare));
var_dump($array, key($array));
$array[] = 43;
var_dump($array);

$array = ['answer' => 42];
next($array);
var_dump(usort($array, $compare));
var_dump($array, key($array));
$array[] = 43;
var_dump($array);

var_dump($calls);

?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  int(42)
}
int(0)
array(2) {
  [0]=>
  int(42)
  [1]=>
  int(43)
}
bool(true)
array(1) {
  [0]=>
  int(42)
}
int(0)
array(2) {
  [0]=>
  int(42)
  [1]=>
  int(43)
}
int(0)
