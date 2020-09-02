--TEST--
Bug #79930: array_merge_recursive() crashes when called with array with single reference
--FILE--
<?php

$a = 'a';
$array = [
    'value' => $a . 'b',
];

// Create rc=1 reference.
array_walk($array, function () {});

$m = array_merge_recursive(['value' => 'a'], $array);

var_dump($a, $array, $m);

?>
--EXPECT--
string(1) "a"
array(1) {
  ["value"]=>
  string(2) "ab"
}
array(1) {
  ["value"]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(2) "ab"
  }
}
