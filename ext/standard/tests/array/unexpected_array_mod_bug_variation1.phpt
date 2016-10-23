--TEST--
Crash when function parameter modified via reference while keeping orig refcount
--FILE--
<?php

$array = array(
    1 => "entry_1",
    2 => "entry_2",
    3 => "entry_3",
    4 => "entry_4",
    5 => "entry_5"
);
usort($array, function($a, $b) use (&$array, &$ref) {
    unset($array[2]);
    $ref = $array;
    return $a <=> $b;
});
var_dump($array);

?>
--EXPECT--
array(5) {
  [0]=>
  string(7) "entry_1"
  [1]=>
  string(7) "entry_2"
  [2]=>
  string(7) "entry_3"
  [3]=>
  string(7) "entry_4"
  [4]=>
  string(7) "entry_5"
}
