--TEST--
Test uasort() function : usage variations - built-in function as 'cmp_function'
--FILE--
<?php
/*
* Passing different built-in library functions in place of 'cmp_function'
*   valid comparison functions: strcmp() & strcasecmp()
*/

echo "*** Testing uasort() : built in function as 'cmp_function' ***\n";
// Initializing variables
$array_arg = array("b" => "Banana", "m" => "Mango", "a" => "apple", "p" => "Pineapple", "o" => "orange");
$builtin_fun_arg = $array_arg;
$languageConstruct_fun_arg = $array_arg;

// Testing library functions as comparison function
echo "-- Testing uasort() with built-in 'cmp_function': strcasecmp() --\n";
var_dump( uasort($builtin_fun_arg, 'strcasecmp') );  // expecting: bool(true)
var_dump($builtin_fun_arg);

echo "-- Testing uasort() with built-in 'cmp_function': strcmp() --\n";
var_dump( uasort($array_arg, 'strcmp') );  // expecting: bool(true)
var_dump($array_arg);

echo "Done"
?>
--EXPECT--
*** Testing uasort() : built in function as 'cmp_function' ***
-- Testing uasort() with built-in 'cmp_function': strcasecmp() --
bool(true)
array(5) {
  ["a"]=>
  string(5) "apple"
  ["b"]=>
  string(6) "Banana"
  ["m"]=>
  string(5) "Mango"
  ["o"]=>
  string(6) "orange"
  ["p"]=>
  string(9) "Pineapple"
}
-- Testing uasort() with built-in 'cmp_function': strcmp() --
bool(true)
array(5) {
  ["b"]=>
  string(6) "Banana"
  ["m"]=>
  string(5) "Mango"
  ["p"]=>
  string(9) "Pineapple"
  ["a"]=>
  string(5) "apple"
  ["o"]=>
  string(6) "orange"
}
Done
