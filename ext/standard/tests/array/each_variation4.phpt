--TEST--
Test each() function : usage variations - Referenced variables
--INI--
allow_call_time_pass_reference=on
--FILE--
<?php
/* Prototype  : array each(array $arr)
 * Description: Return the currently pointed key..value pair in the passed array,
 * and advance the pointer to the next element 
 * Source code: Zend/zend_builtin_functions.c
 */

/*
 * Test behaviour of each() when:
 * 1. Passed an array made up of referenced variables
 * 2. Passed an array as $arr argument by reference
 */

echo "*** Testing each() : usage variations ***\n";

echo "\n-- Array made up of referenced variables: --\n";
$val1 = 'foo';
$val2 = 'bar';

$arr1 = array('one' => &$val1, &$val2);

echo "-- Call each until at the end of the array: --\n";
var_dump( each($arr1) );
var_dump( each($arr1) );
var_dump( each($arr1) );


echo "\n-- Pass an array by reference to each(): --\n";
$arr2 = array('zero', 'one', 'two');

var_dump( each(&$arr2) );
echo "-- Check original array: --\n";
var_dump($arr2);

echo "Done";
?>

--EXPECTF--
*** Testing each() : usage variations ***

-- Array made up of referenced variables: --
-- Call each until at the end of the array: --
array(4) {
  [1]=>
  string(3) "foo"
  ["value"]=>
  string(3) "foo"
  [0]=>
  string(3) "one"
  ["key"]=>
  string(3) "one"
}
array(4) {
  [1]=>
  string(3) "bar"
  ["value"]=>
  string(3) "bar"
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}
bool(false)

-- Pass an array by reference to each(): --
array(4) {
  [1]=>
  string(4) "zero"
  ["value"]=>
  string(4) "zero"
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}
-- Check original array: --
array(3) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
Done
