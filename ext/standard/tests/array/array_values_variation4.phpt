--TEST--
Test array_values() function : usage variations - multi-dimensional arrays
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array
 * Source code: ext/standard/array.c
 */

/*
 * Test array_values when:
 * 1. Passed a two-dimensional array as $input argument
 * 2. Passed a sub-array as $input argument
 * 3. Passed an infinitely recursive multi-dimensional array
 */

echo "*** Testing array_values() : usage variations ***\n";

$input = array ('zero' => 'zero', 'un' => 'one', 'sub' => array (1, 2, 3));

echo "\n-- Array values of a two-dimensional array --\n";
var_dump(array_values($input));

echo "\n-- Array values of a sub-array --\n";
var_dump(array_values($input['sub']));

// get an infinitely recursive array
$input[] = &$input;
echo "\n-- Array values of an infinitely recursive array --\n";
var_dump(array_values($input));

echo "Done";
?>
--EXPECT--
*** Testing array_values() : usage variations ***

-- Array values of a two-dimensional array --
array(3) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Array values of a sub-array --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

-- Array values of an infinitely recursive array --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  [3]=>
  &array(4) {
    ["zero"]=>
    string(4) "zero"
    ["un"]=>
    string(3) "one"
    ["sub"]=>
    array(3) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
    }
    [0]=>
    *RECURSION*
  }
}
Done
