--TEST--
Test array_chunk() function : usage variations - different 'size' values
--FILE--
<?php
/*
 * Testing array_chunk() function with following conditions
 *   1. -ve size value
 *   2. size value is more than the no. of elements in the input array
 *   3. size value is zero
 *   4. Decimal size value
*/

echo "*** Testing array_chunk() : usage variations ***\n";

// input array
$input_array = array(1, 2, 3);

// different magnitude's
$sizes = array(-1, count($input_array) + 1, 0, 1);

// loop through the array for size argument
foreach ($sizes as $size){
    echo "\n-- Testing array_chunk() when size = $size --\n";
    try {
        var_dump( array_chunk($input_array, $size) );
    } catch (\ValueError $e) {
        echo $e->getMessage() . "\n";
    }
    try {
        var_dump( array_chunk($input_array, $size, true) );
    } catch (\ValueError $e) {
        echo $e->getMessage() . "\n";
    }
    try {
        var_dump( array_chunk($input_array, $size, false) );
    } catch (\ValueError $e) {
        echo $e->getMessage() . "\n";
    }
}
?>
--EXPECT--
*** Testing array_chunk() : usage variations ***

-- Testing array_chunk() when size = -1 --
array_chunk(): Argument #2 ($length) must be greater than 0
array_chunk(): Argument #2 ($length) must be greater than 0
array_chunk(): Argument #2 ($length) must be greater than 0

-- Testing array_chunk() when size = 4 --
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}

-- Testing array_chunk() when size = 0 --
array_chunk(): Argument #2 ($length) must be greater than 0
array_chunk(): Argument #2 ($length) must be greater than 0
array_chunk(): Argument #2 ($length) must be greater than 0

-- Testing array_chunk() when size = 1 --
array(3) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
  [2]=>
  array(1) {
    [0]=>
    int(3)
  }
}
array(3) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [1]=>
    int(2)
  }
  [2]=>
  array(1) {
    [2]=>
    int(3)
  }
}
array(3) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(1) {
    [0]=>
    int(2)
  }
  [2]=>
  array(1) {
    [0]=>
    int(3)
  }
}
