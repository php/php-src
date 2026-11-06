--TEST--
Test array_partition() function : basic functionality
--FILE--
<?php

echo "*** Basic split ***\n";
var_dump(array_partition([1, 2, 3, 4, 5], fn($v) => $v % 2 === 1));

echo "*** Empty array ***\n";
var_dump(array_partition([], fn($v) => true));

echo "*** All match ***\n";
var_dump(array_partition([1, 2], fn($v) => true));

echo "*** None match ***\n";
var_dump(array_partition([1, 2], fn($v) => false));

echo "*** Callback return value uses standard truthiness ***\n";
var_dump(array_partition(['a', '', '0', 'b'], fn($v) => $v));

echo "*** Destructuring ***\n";
[$adults, $minors] = array_partition(['ann' => 34, 'bob' => 12, 'cy' => 18], fn($age) => $age >= 18);
var_dump($adults, $minors);

echo "*** Callback is required ***\n";
try {
    array_partition([1], null);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Basic split ***
array(2) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [2]=>
    int(3)
    [4]=>
    int(5)
  }
  [1]=>
  array(2) {
    [1]=>
    int(2)
    [3]=>
    int(4)
  }
}
*** Empty array ***
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(0) {
  }
}
*** All match ***
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(0) {
  }
}
*** None match ***
array(2) {
  [0]=>
  array(0) {
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
*** Callback return value uses standard truthiness ***
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [3]=>
    string(1) "b"
  }
  [1]=>
  array(2) {
    [1]=>
    string(0) ""
    [2]=>
    string(1) "0"
  }
}
*** Destructuring ***
array(2) {
  ["ann"]=>
  int(34)
  ["cy"]=>
  int(18)
}
array(1) {
  ["bob"]=>
  int(12)
}
*** Callback is required ***
TypeError: array_partition(): Argument #2 ($callback) must be a valid callback, no array or string given
