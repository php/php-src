--TEST--
Test shuffle() function : usage variation - with MultiDimensional array
--FILE--
<?php
/*
* Test behaviour of shuffle() function when multi-dimensional array is
* passed to 'array_arg' argument
*/

echo "*** Testing shuffle() : with multi-dimensional array ***\n";

// initialise the multi-dimensional array
$array_arg = array(
  array(1, 2, 3),
  array(4, 5, 6),
  array(7, 8, 9),
  array(10000, 20000000, 30000000),
  array(0, 0, 0),
  array(012, 023, 034),
  array(0x1, 0x0, 0xa)

);

// calling shuffle() function with multi-dimensional array
var_dump( shuffle($array_arg) );
echo "\nThe output array is:\n";
var_dump( $array_arg );


// looping to test shuffle() with each sub-array in the multi-dimensional array
echo "\n*** Testing shuffle() with arrays having different types of values ***\n";
$counter = 1;
for($i=0; $i<=6; $i++) {
  echo "\n-- Iteration $counter --\n";
  var_dump( shuffle($array_arg[$i]) );
  echo "\nThe output array is:\n";
  var_dump( $array_arg[$i] );
  $counter++;
}

echo "Done";
?>
--EXPECTF--
*** Testing shuffle() : with multi-dimensional array ***
bool(true)

The output array is:
array(7) {
  [0]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    int(%d)
    [2]=>
    int(%d)
  }
  [1]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    int(%d)
    [2]=>
    int(%d)
  }
  [2]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    int(%d)
    [2]=>
    int(%d)
  }
  [3]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    int(%d)
    [2]=>
    int(%d)
  }
  [4]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    int(%d)
    [2]=>
    int(%d)
  }
  [5]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    int(%d)
    [2]=>
    int(%d)
  }
  [6]=>
  array(3) {
    [0]=>
    int(%d)
    [1]=>
    int(%d)
    [2]=>
    int(%d)
  }
}

*** Testing shuffle() with arrays having different types of values ***

-- Iteration 1 --
bool(true)

The output array is:
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 2 --
bool(true)

The output array is:
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 3 --
bool(true)

The output array is:
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 4 --
bool(true)

The output array is:
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 5 --
bool(true)

The output array is:
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 6 --
bool(true)

The output array is:
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}

-- Iteration 7 --
bool(true)

The output array is:
array(3) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
  [2]=>
  int(%d)
}
Done
