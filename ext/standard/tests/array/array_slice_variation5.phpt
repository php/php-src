--TEST--
Test array_slice() function : usage variations - Pass different integers as $offset argument
--FILE--
<?php
/* Prototype  : array array_slice(array $input, int $offset [, int $length [, bool $preserve_keys]])
 * Description: Returns elements specified by offset and length
 * Source code: ext/standard/array.c
 */

/*
 * Pass different integers as $offset argument to test how array_slice() behaves
 */

echo "*** Testing array_slice() : usage variations ***\n";

$input = array ('one' => 1, 2 => 'two', 'three', 9 => 'nine', 'ten' => 10);

for ($i = -7; $i <= 7; $i++) {
	echo "\n-- \$offset is $i --\n";
	var_dump(array_slice($input, $i));
}
echo "\n-- \$offset is maximum integer value --\n";
var_dump(array_slice($input, PHP_INT_MAX));

echo "\n-- \$offset is minimum integer value --\n";
var_dump(array_slice($input, -PHP_INT_MAX));

echo "Done";
?>
--EXPECT--
*** Testing array_slice() : usage variations ***

-- $offset is -7 --
array(5) {
  ["one"]=>
  int(1)
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is -6 --
array(5) {
  ["one"]=>
  int(1)
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is -5 --
array(5) {
  ["one"]=>
  int(1)
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is -4 --
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

-- $offset is -3 --
array(3) {
  [0]=>
  string(5) "three"
  [1]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is -2 --
array(2) {
  [0]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is -1 --
array(1) {
  ["ten"]=>
  int(10)
}

-- $offset is 0 --
array(5) {
  ["one"]=>
  int(1)
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is 1 --
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

-- $offset is 2 --
array(3) {
  [0]=>
  string(5) "three"
  [1]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is 3 --
array(2) {
  [0]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}

-- $offset is 4 --
array(1) {
  ["ten"]=>
  int(10)
}

-- $offset is 5 --
array(0) {
}

-- $offset is 6 --
array(0) {
}

-- $offset is 7 --
array(0) {
}

-- $offset is maximum integer value --
array(0) {
}

-- $offset is minimum integer value --
array(5) {
  ["one"]=>
  int(1)
  [0]=>
  string(3) "two"
  [1]=>
  string(5) "three"
  [2]=>
  string(4) "nine"
  ["ten"]=>
  int(10)
}
Done
