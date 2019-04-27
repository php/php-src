--TEST--
Test array_fill() function : usage variations  - unexpected values for 'val' argument
--FILE--
<?php
/* Prototype  : array array_fill(int $start_key, int $num, mixed $val)
 * Description: Create an array containing num elements starting with index start_key each initialized to val
 * Source code: ext/standard/array.c
 */

/*
 * testing array_fill() by passing different unexpected values for 'val' argument
 */

echo "*** Testing array_fill() : usage variations ***\n";

// Initialise function arguments not being substituted
$start_key = 0;
$num = 2;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// define a class
class test
{
  var $t = 10;
  function __toString()
  {
    return "testObject";
  }
}


//array of different values for 'val' argument
$values = array(
            // empty string
  /* 1  */  "",
            '',
            // objects
  /* 3  */  new test(),

            // undefined variable
            @$undefined_var,

            // unset variable
  /* 5  */  @$unset_var,
);

// loop through each element of the array for 'val' argument
// check the working of array_fill()
echo "--- Testing array_fill() with different values for 'val' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++)
{
  echo "-- Iteration $counter --\n";
  $val = $values[$index];

  var_dump( array_fill($start_key , $num , $val) );

  $counter++;
}

echo"Done";
?>
--EXPECTF--
*** Testing array_fill() : usage variations ***
--- Testing array_fill() with different values for 'val' argument ---
-- Iteration 1 --
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
-- Iteration 2 --
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
-- Iteration 3 --
array(2) {
  [0]=>
  object(test)#%d (1) {
    ["t"]=>
    int(10)
  }
  [1]=>
  object(test)#%d (1) {
    ["t"]=>
    int(10)
  }
}
-- Iteration 4 --
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
-- Iteration 5 --
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
Done
