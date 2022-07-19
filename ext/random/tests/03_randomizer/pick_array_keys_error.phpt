--TEST--
Random: Randomizer: pickArrayKeys error pattern
--FILE--
<?php

try {
    var_dump((new \Random\Randomizer())->pickArrayKeys([], 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump((new \Random\Randomizer())->pickArrayKeys(range(1, 3), 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump((new \Random\Randomizer())->pickArrayKeys(range(1, 3), -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    var_dump((new \Random\Randomizer())->pickArrayKeys(range(1, 3), 10));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump((new \Random\Randomizer())->pickArrayKeys(range(1, 3), 3));
var_dump((new \Random\Randomizer())->pickArrayKeys(range(1, 3), 2));

?>
--EXPECTF--
Random\Randomizer::pickArrayKeys(): Argument #1 ($array) cannot be empty
Random\Randomizer::pickArrayKeys(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
Random\Randomizer::pickArrayKeys(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
Random\Randomizer::pickArrayKeys(): Argument #2 ($num) must be between 1 and the number of elements in argument #1 ($array)
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
