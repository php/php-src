--TEST--
Test array_filter() function : basic functionality
--FILE--
<?php
echo "*** Testing array_filter() : basic functionality ***\n";


// Initialise all required variables
$input = array(1, 2, 3, 0, -1);  // 0 will be considered as FALSE and removed in default callback

function even($input)
{
  return ($input % 2 == 0);
}

// with all possible arguments
var_dump( array_filter($input,"even") );

// with default arguments
var_dump( array_filter($input) );
// same as with default arguments
var_dump( array_filter($input, null) );

echo "Done"
?>
--EXPECT--
*** Testing array_filter() : basic functionality ***
array(2) {
  [1]=>
  int(2)
  [3]=>
  int(0)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [4]=>
  int(-1)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [4]=>
  int(-1)
}
Done
