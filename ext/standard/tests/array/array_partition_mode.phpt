--TEST--
Test array_partition() function : ARRAY_FILTER_USE_KEY and ARRAY_FILTER_USE_BOTH
--FILE--
<?php

$input = ['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4];

echo "*** ARRAY_FILTER_USE_VALUE (explicit) ***\n";
var_dump(array_partition($input, fn($v) => $v > 2, ARRAY_FILTER_USE_VALUE));

echo "*** ARRAY_FILTER_USE_KEY ***\n";
var_dump(array_partition($input, fn($k) => $k === 'b' || $k === 'd', ARRAY_FILTER_USE_KEY));

echo "*** ARRAY_FILTER_USE_BOTH ***\n";
var_dump(array_partition($input, fn($v, $k) => $k === 'a' || $v === 4, ARRAY_FILTER_USE_BOTH));

echo "*** ARRAY_FILTER_USE_BOTH receives (value, key) ***\n";
array_partition([5 => 'x', 'k' => 'y'], function ($v, $k) {
    var_dump($v, $k);
    return true;
}, ARRAY_FILTER_USE_BOTH);

echo "*** Same result as array_filter for every mode ***\n";
$callbacks = [
    ARRAY_FILTER_USE_VALUE => fn($v) => $v % 2,
    ARRAY_FILTER_USE_KEY => fn($k) => $k > 'b',
    ARRAY_FILTER_USE_BOTH => fn($v, $k) => $v === 1 || $k === 'c',
];
foreach ($callbacks as $mode => $callback) {
    [$matching, $nonMatching] = array_partition($input, $callback, $mode);
    var_dump($matching === array_filter($input, $callback, $mode));
    var_dump($nonMatching === array_diff_key($input, $matching));
}

?>
--EXPECT--
*** ARRAY_FILTER_USE_VALUE (explicit) ***
array(2) {
  [0]=>
  array(2) {
    ["c"]=>
    int(3)
    ["d"]=>
    int(4)
  }
  [1]=>
  array(2) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(2)
  }
}
*** ARRAY_FILTER_USE_KEY ***
array(2) {
  [0]=>
  array(2) {
    ["b"]=>
    int(2)
    ["d"]=>
    int(4)
  }
  [1]=>
  array(2) {
    ["a"]=>
    int(1)
    ["c"]=>
    int(3)
  }
}
*** ARRAY_FILTER_USE_BOTH ***
array(2) {
  [0]=>
  array(2) {
    ["a"]=>
    int(1)
    ["d"]=>
    int(4)
  }
  [1]=>
  array(2) {
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
  }
}
*** ARRAY_FILTER_USE_BOTH receives (value, key) ***
string(1) "x"
int(5)
string(1) "y"
string(1) "k"
*** Same result as array_filter for every mode ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
