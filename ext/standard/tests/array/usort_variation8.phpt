--TEST--
Test usort() function : usage variations - use built in functions as $cmp_function arg
--FILE--
<?php
/* Prototype  : bool usort(array $array_arg, string $cmp_function)
 * Description: Sort an array by values using a user-defined comparison function 
 * Source code: ext/standard/array.c
 */

/*
 * Test usort() when comparison function is:
 * 1. a built in comparison function
 * 2. a language construct
 */

echo "*** Testing usort() : usage variation ***\n";

// Initializing variables
$array_arg = array("b" => "Banana", "m" => "Mango", "a" => "apple", 
                   "p" => "Pineapple", "o" => "orange");

// Testing library functions as comparison function 
echo "\n-- Testing usort() with built-in 'cmp_function': strcasecmp() --\n";
$temp_array1 = $array_arg;
var_dump( usort($temp_array1, 'strcasecmp') );
var_dump($temp_array1);

echo "\n-- Testing usort() with built-in 'cmp_function': strcmp() --\n";
$temp_array2 = $array_arg;
var_dump( usort($temp_array2, 'strcmp') );
var_dump($temp_array2);

// Testing with language construct as comparison function
echo "\n-- Testing usort() with language construct as 'cmp_function' --\n";
$temp_array3 = $array_arg;
var_dump( usort($temp_array3, 'echo') );

echo "\n-- Testing usort() with language construct as 'cmp_function' --\n";
$temp_array4 = $array_arg;
var_dump( usort($temp_array4, 'exit') );
?>
===DONE===
--EXPECTF--
*** Testing usort() : usage variation ***

-- Testing usort() with built-in 'cmp_function': strcasecmp() --
bool(true)
array(5) {
  [0]=>
  string(5) "apple"
  [1]=>
  string(6) "Banana"
  [2]=>
  string(5) "Mango"
  [3]=>
  string(6) "orange"
  [4]=>
  string(9) "Pineapple"
}

-- Testing usort() with built-in 'cmp_function': strcmp() --
bool(true)
array(5) {
  [0]=>
  string(6) "Banana"
  [1]=>
  string(5) "Mango"
  [2]=>
  string(9) "Pineapple"
  [3]=>
  string(5) "apple"
  [4]=>
  string(6) "orange"
}

-- Testing usort() with language construct as 'cmp_function' --

Warning: usort(): Invalid comparison function in %s on line %d
bool(false)

-- Testing usort() with language construct as 'cmp_function' --

Warning: usort(): Invalid comparison function in %s on line %d
bool(false)
===DONE===
