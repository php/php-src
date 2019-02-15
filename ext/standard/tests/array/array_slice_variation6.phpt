--TEST--
Test array_slice() function : usage variations - pass different int values as $length arg
--FILE--
<?php
/* Prototype  : array array_slice(array $input, int $offset [, int $length [, bool $preserve_keys]])
 * Description: Returns elements specified by offset and length
 * Source code: ext/standard/array.c
 */

/*
 * Pass different integer values as $length argument to array_slice() to test behaviour
 */

echo "*** Testing array_slice() : usage variations ***\n";

$input = array ('one' => 1, 2 => 'two', 'three', 9 => 'nine', 'ten' => 10);
$offset = 1;

for ($i = -6; $i <= 6; $i++) {
	echo "\n-- \$length is $i --\n";
	var_dump(array_slice($input, $offset, $i));
}
echo "\n-- \$length is maximum integer value --\n";
var_dump(array_slice($input, $offset, PHP_INT_MAX));

echo "\n-- \$length is minimum integer value --\n";
var_dump(array_slice($input, $offset, -PHP_INT_MAX));

echo "Done";
?>
--EXPECT--
*** Testing array_slice() : usage variations ***

-- $length is -6 --
array(0) {
}

-- $length is -5 --
array(0) {
}

-- $length is -4 --
array(0) {
}

-- $length is -3 --
array(1) {
  [0]=>
  string(3) "two"
}

-- $length is -2 --
array(2) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
}

-- $length is -1 --
array(3) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
}

-- $length is 0 --
array(0) {
}

-- $length is 1 --
array(1) {
  [0]=>
  string(3) "two"
}

-- $length is 2 --
array(2) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
}

-- $length is 3 --
array(3) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
}

-- $length is 4 --
array(4) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $length is 5 --
array(4) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $length is 6 --
array(4) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $length is maximum integer value --
array(4) {
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $length is minimum integer value --
array(0) {
}
Done
