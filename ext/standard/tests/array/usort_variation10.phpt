--TEST--
Test usort() function : usage variations - duplicate keys and values
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function
 * Source code: ext/standard/array.c
 */

/*
 * Pass an array with duplicate keys and values to usort() to test behaviour
 */

echo "*** Testing usort() : usage variation ***\n";

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

// Array with duplicate string and integer keys and values
$array_arg = array(0 => 2,     "a" => 8, "d" => 9,
                   3 => 3,     5 => 2,   "o" => 6,
                   "z" => -99, 0 => 1,   "z" => 3);

echo "\n-- Array with duplicate keys --\n";
var_dump( usort($array_arg, 'cmp') );
var_dump($array_arg);

// Array with default and assigned keys
$array_arg = array(0 => "Banana", 1 => "Mango", "Orange", 2 => "Apple", "Pineapple");

echo "\n-- Array with default/assigned keys --\n";
var_dump( usort($array_arg, 'cmp') );
var_dump($array_arg);
?>
===DONE===
--EXPECT--
*** Testing usort() : usage variation ***

-- Array with duplicate keys --
bool(true)
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(3)
  [4]=>
  int(6)
  [5]=>
  int(8)
  [6]=>
  int(9)
}

-- Array with default/assigned keys --
bool(true)
array(4) {
  [0]=>
  string(5) "Apple"
  [1]=>
  string(6) "Banana"
  [2]=>
  string(5) "Mango"
  [3]=>
  string(9) "Pineapple"
}
===DONE===
