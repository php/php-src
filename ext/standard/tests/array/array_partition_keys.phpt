--TEST--
Test array_partition() function : key preservation
--FILE--
<?php

echo "*** String keys ***\n";
var_dump(array_partition(['a' => 1, 'b' => 2, 'c' => 3], fn($v) => $v >= 2));

echo "*** Non-sequential int keys ***\n";
var_dump(array_partition([10 => 'x', 5 => 'y', -3 => 'z'], fn($v) => $v !== 'y'));

echo "*** Mixed keys ***\n";
var_dump(array_partition([0 => 'zero', 'one' => 1, 7 => 'seven', 'two' => 2], fn($v) => is_string($v)));

?>
--EXPECT--
*** String keys ***
array(2) {
  [0]=>
  array(2) {
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
  }
  [1]=>
  array(1) {
    ["a"]=>
    int(1)
  }
}
*** Non-sequential int keys ***
array(2) {
  [0]=>
  array(2) {
    [10]=>
    string(1) "x"
    [-3]=>
    string(1) "z"
  }
  [1]=>
  array(1) {
    [5]=>
    string(1) "y"
  }
}
*** Mixed keys ***
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "zero"
    [7]=>
    string(5) "seven"
  }
  [1]=>
  array(2) {
    ["one"]=>
    int(1)
    ["two"]=>
    int(2)
  }
}
