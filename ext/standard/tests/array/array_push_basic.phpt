--TEST--
Test array_push() function : basic functionality 
--FILE--
<?php
/* Prototype  : int array_push(array $stack, mixed $var [, mixed $...])
 * Description: Pushes elements onto the end of the array 
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of array_push with indexed and associative arrays
 */

echo "*** Testing array_push() : basic functionality ***\n";

$array = array ('zero', 'one', 'two');
$var1 = 'three';
$var2 = 'four';

echo "\n-- Push values onto an indexed array --\n";
var_dump(array_push($array, $var1, $var2));
var_dump($array);

$array_assoc = array ('one' => 'un', 'two' => 'deux');

echo "\n-- Push values onto an associative array --\n";
var_dump(array_push($array_assoc, $var1, $var2));
var_dump($array_assoc);

echo "Done";
?>

--EXPECTF--
*** Testing array_push() : basic functionality ***

-- Push values onto an indexed array --
int(5)
array(5) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
  [4]=>
  string(4) "four"
}

-- Push values onto an associative array --
int(4)
array(4) {
  ["one"]=>
  string(2) "un"
  ["two"]=>
  string(4) "deux"
  [0]=>
  string(5) "three"
  [1]=>
  string(4) "four"
}
Done