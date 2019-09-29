--TEST--
array_rand() tests
--FILE--
<?php

try {
    var_dump(array_rand(array()));
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(), 0));
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(1,2,3), 0));
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(1,2,3), -1));
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump(array_rand(array(1,2,3), 10));
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump(array_rand(array(1,2,3), 3));
var_dump(array_rand(array(1,2,3), 2));

echo "Done\n";
?>
--EXPECTF--
Array is empty
Array is empty
Second argument has to be between 1 and the number of elements in the array
Second argument has to be between 1 and the number of elements in the array
Second argument has to be between 1 and the number of elements in the array
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
Done
