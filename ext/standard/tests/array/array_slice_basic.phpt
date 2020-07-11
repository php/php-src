--TEST--
Test array_slice() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of array_slice()
 */

echo "*** Testing array_slice() : basic functionality ***\n";


$input = array('one' => 1, 'two' => 2, 3, 23 => 4);
$offset = 2;
$length = 2;
$preserve_keys = true;

// Calling array_slice() with all possible arguments
echo "\n-- All arguments --\n";
var_dump( array_slice($input, $offset, $length, $preserve_keys) );

// Calling array_slice() with mandatory arguments
echo "\n-- Mandatory arguments --\n";
var_dump( array_slice($input, $offset) );

echo "Done";
?>
--EXPECT--
*** Testing array_slice() : basic functionality ***

-- All arguments --
array(2) {
  [0]=>
  int(3)
  [23]=>
  int(4)
}

-- Mandatory arguments --
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
Done
