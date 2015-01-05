--TEST--
Test uasort() function : usage variations - anonymous function as 'cmp_function'
--FILE--
<?php
/* Prototype  : bool uasort(array $array_arg, string $cmp_function)
 * Description: Sort an array with a user-defined comparison function and maintain index association 
 * Source code: ext/standard/array.c
*/

/*
* Passing different anonymous functions as 'cmp_function'
*   arguments passed by value
*   arguments passed by reference
*/

echo "*** Testing uasort() : anonymous function as 'cmp_function' ***\n";

$cmp_function = 'if($value1 == $value2) {return 0;} else if($value1 > $value2) {return 1;} else{return -1;}';

$array_arg = array(0 => 100, 1 => 3, 2 => -70, 3 => 24, 4 => 90);
echo "-- Anonymous 'cmp_function' with parameters passed by value --\n";
var_dump( uasort($array_arg, create_function('$value1, $value2',$cmp_function) ) );
var_dump($array_arg);

$array_arg = array("b" => "Banana", "m" => "Mango", "a" => "Apple", "p" => "Pineapple");
echo "-- Anonymous 'cmp_function' with parameters passed by reference --\n";
var_dump( uasort($array_arg, create_function('&$value1, &$value2', $cmp_function) ) );
var_dump($array_arg);

echo "Done"
?>
--EXPECTF--
*** Testing uasort() : anonymous function as 'cmp_function' ***
-- Anonymous 'cmp_function' with parameters passed by value --
bool(true)
array(5) {
  [2]=>
  int(-70)
  [1]=>
  int(3)
  [3]=>
  int(24)
  [4]=>
  int(90)
  [0]=>
  int(100)
}
-- Anonymous 'cmp_function' with parameters passed by reference --
bool(true)
array(4) {
  ["a"]=>
  string(5) "Apple"
  ["b"]=>
  string(6) "Banana"
  ["m"]=>
  string(5) "Mango"
  ["p"]=>
  string(9) "Pineapple"
}
Done
