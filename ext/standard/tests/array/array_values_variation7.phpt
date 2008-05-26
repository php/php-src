--TEST--
Test array_values() function : usage variations - Internal order check
--FILE--
<?php
/* Prototype  : array array_values(array $input)
 * Description: Return just the values from the input array 
 * Source code: ext/standard/array.c
 */

/*
 * Check that array_values is re-assigning keys according to the internal order of the array,
 * and is not dependant on the \$input argument's keys
 */

echo "*** Testing array_values() : usage variations ***\n";

// populate array with 'default' keys in reverse order
$input = array(3 => 'three', 2 => 'two', 1 => 'one', 0 => 'zero');

echo "\n-- \$input argument: --\n";
var_dump($input);

echo "\n-- Result of array_values() --\n";
var_dump(array_values($input));

echo "Done";
?>

--EXPECT--
*** Testing array_values() : usage variations ***

-- $input argument: --
array(4) {
  [3]=>
  unicode(5) "three"
  [2]=>
  unicode(3) "two"
  [1]=>
  unicode(3) "one"
  [0]=>
  unicode(4) "zero"
}

-- Result of array_values() --
array(4) {
  [0]=>
  unicode(5) "three"
  [1]=>
  unicode(3) "two"
  [2]=>
  unicode(3) "one"
  [3]=>
  unicode(4) "zero"
}
Done
