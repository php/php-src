--TEST--
Test usort() function : basic functionality
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of usort() with indexed and associative arrays
 */

echo "*** Testing usort() : basic functionality ***\n";

function cmp($value1, $value2)
{
  if($value1 == $value2) {
    return 0;
  }
  else if($value1 > $value2) {
    return 1;
  }
  else
    return -1;
}

// Int array with default keys
$int_values = array(1, 8, 9, 3, 2, 6, 7);

echo "\n-- Numeric array with default keys --\n";
var_dump( usort($int_values, 'cmp') );
var_dump($int_values);

// String array with default keys
$string_values = array("This", "is", 'a', "test");

echo "\n-- String array with default keys --\n";
var_dump( usort($string_values, 'cmp') );
var_dump($string_values);

// Associative array with numeric keys
$numeric_key_arg = array(1=> 1, 2 => 2, 3 => 7, 5 => 4, 4 => 9);

echo "\n-- Associative array with numeric keys --\n";
var_dump( usort($numeric_key_arg, 'cmp') );
var_dump($numeric_key_arg);

// Associative array with string keys
$string_key_arg = array('one' => 4, 'two' => 2, 'three' => 1, 'four' => 10);

echo "\n-- Associative array with string keys --\n";
var_dump( usort($string_key_arg, 'cmp') );
var_dump($string_key_arg);
?>
===DONE===
--EXPECT--
*** Testing usort() : basic functionality ***

-- Numeric array with default keys --
bool(true)
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(6)
  [4]=>
  int(7)
  [5]=>
  int(8)
  [6]=>
  int(9)
}

-- String array with default keys --
bool(true)
array(4) {
  [0]=>
  string(4) "This"
  [1]=>
  string(1) "a"
  [2]=>
  string(2) "is"
  [3]=>
  string(4) "test"
}

-- Associative array with numeric keys --
bool(true)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
  [3]=>
  int(7)
  [4]=>
  int(9)
}

-- Associative array with string keys --
bool(true)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(4)
  [3]=>
  int(10)
}
===DONE===
