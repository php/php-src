--TEST--
array_rand() tests
--FILE--
<?php

try {
    var_dump(array_rand(array()));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(), 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(1,2,3), 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(1,2,3), -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(1,2,3), 10));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(array_rand(array(1,2,3), 3));
var_dump(array_rand(array(1,2,3), 2));

?>
--EXPECTF--
array_rand(): Argument #1 ($array) cannot be empty
array_rand(): Argument #1 ($array) cannot be empty
array_rand(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
array_rand(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
array_rand(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}
array(2) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
}
