--TEST--
Test array_change_key_case() function : usage variations - multidimensional arrays
--FILE--
<?php
/* Prototype  : array array_change_key_case(array $input [, int $case])
 * Description: Retuns an array with all string keys lowercased [or uppercased] 
 * Source code: ext/standard/array.c
 */

/*
 * Test how array_change_key_case() converts keys in multi-dimensional arrays
 */

echo "*** Testing array_change_key_case() : usage variations ***\n";

$input = array('English' => array('one' => 1, 'two' => 2, 'three' => 3),
               'French'  => array('un' => 1, 'deux' => 2, 'trois' => 3),
               'German'  => array('eins' => 1, 'zwei' => 2, 'drei' => 3));

echo "\n-- Pass a two-dimensional array as \$input argument --\n";
var_dump(array_change_key_case($input, CASE_UPPER));

echo "\n-- Pass a sub-arry as \$input argument --\n";
var_dump(array_change_key_case($input['English'], CASE_UPPER));

echo "Done";
?>

--EXPECTF--
*** Testing array_change_key_case() : usage variations ***

-- Pass a two-dimensional array as $input argument --
array(3) {
  ["ENGLISH"]=>
  array(3) {
    ["one"]=>
    int(1)
    ["two"]=>
    int(2)
    ["three"]=>
    int(3)
  }
  ["FRENCH"]=>
  array(3) {
    ["un"]=>
    int(1)
    ["deux"]=>
    int(2)
    ["trois"]=>
    int(3)
  }
  ["GERMAN"]=>
  array(3) {
    ["eins"]=>
    int(1)
    ["zwei"]=>
    int(2)
    ["drei"]=>
    int(3)
  }
}

-- Pass a sub-arry as $input argument --
array(3) {
  ["ONE"]=>
  int(1)
  ["TWO"]=>
  int(2)
  ["THREE"]=>
  int(3)
}
Done