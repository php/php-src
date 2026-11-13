--TEST--
Test array_partition() function : callback receives the value and the key
--FILE--
<?php

$input = ['a' => 1, 'b' => 2, 5 => 3, 6 => 4];

echo "*** Callback receives (value, key) ***\n";
array_partition($input, function ($value, $key) {
    var_dump($value, $key);
    return true;
});

echo "*** Partition by key ***\n";
var_dump(array_partition($input, fn($value, $key) => is_string($key)));

echo "*** Partition by value and key ***\n";
var_dump(array_partition($input, fn($value, $key) => $key === 'a' || $value === 4));

echo "*** Same result as array_filter() with ARRAY_FILTER_USE_BOTH ***\n";
$callback = fn($value, $key) => $value % 2 === 1 || $key === 6;
[$matching, $nonMatching] = array_partition($input, $callback);
var_dump($matching === array_filter($input, $callback, ARRAY_FILTER_USE_BOTH));
var_dump($nonMatching === array_diff_key($input, $matching));

echo "*** Internal functions with a fixed arity receive both arguments ***\n";
try {
    array_partition(['x'], 'is_string');
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "*** There is no \$mode argument ***\n";
try {
    array_partition($input, fn($value) => true, ARRAY_FILTER_USE_KEY);
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Callback receives (value, key) ***
int(1)
string(1) "a"
int(2)
string(1) "b"
int(3)
int(5)
int(4)
int(6)
*** Partition by key ***
array(2) {
  [0]=>
  array(2) {
    ["a"]=>
    int(1)
    ["b"]=>
    int(2)
  }
  [1]=>
  array(2) {
    [5]=>
    int(3)
    [6]=>
    int(4)
  }
}
*** Partition by value and key ***
array(2) {
  [0]=>
  array(2) {
    ["a"]=>
    int(1)
    [6]=>
    int(4)
  }
  [1]=>
  array(2) {
    ["b"]=>
    int(2)
    [5]=>
    int(3)
  }
}
*** Same result as array_filter() with ARRAY_FILTER_USE_BOTH ***
bool(true)
bool(true)
*** Internal functions with a fixed arity receive both arguments ***
ArgumentCountError: is_string() expects exactly 1 argument, 2 given
*** There is no $mode argument ***
ArgumentCountError: array_partition() expects exactly 2 arguments, 3 given
