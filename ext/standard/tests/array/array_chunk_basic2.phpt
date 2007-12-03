--TEST--
Test array_chunk() function : basic functionality - 'preserve_keys' as true/false 
--FILE--
<?php
/* Prototype  : array array_chunk(array $array, int $size [, bool $preserve_keys])
 * Description: Split array into chunks
 *              Chunks an array into size  large chunks.
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_chunk() : basic functionality ***\n";
$size = 2;

$input_arrays = array(
  // array with default keys - numeric values
  array(1, 2, 3, 4, 5),

  // array with default keys - string values
  array('value1', "value2", "value3"),

  // associative arrays - key as string
  array('key1' => 1, "key2" => 2, "key3" => 3),
 
  // associative arrays - key as numeric
  array(1 => 'one', 2 => "two", 3 => "three"),

  // array containing elements with/without keys
  array(1 => 'one','two', 3 => 'three', 4, "five" => 5)
); 

$count = 1;
// loop through each element of the array for input
foreach ($input_arrays as $input_array){ 
  echo "\n-- Iteration $count --\n";  
  var_dump( array_chunk($input_array, $size, true) );
  var_dump( array_chunk($input_array, $size, false) );
  $count++;
}

echo "Done"
?>
--EXPECTF--
*** Testing array_chunk() : basic functionality ***

-- Iteration 1 --
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [2]=>
    int(3)
    [3]=>
    int(4)
  }
  [2]=>
  array(1) {
    [4]=>
    int(5)
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(2) {
    [0]=>
    int(3)
    [1]=>
    int(4)
  }
  [2]=>
  array(1) {
    [0]=>
    int(5)
  }
}

-- Iteration 2 --
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(6) "value1"
    [1]=>
    string(6) "value2"
  }
  [1]=>
  array(1) {
    [2]=>
    string(6) "value3"
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(6) "value1"
    [1]=>
    string(6) "value2"
  }
  [1]=>
  array(1) {
    [0]=>
    string(6) "value3"
  }
}

-- Iteration 3 --
array(2) {
  [0]=>
  array(2) {
    ["key1"]=>
    int(1)
    ["key2"]=>
    int(2)
  }
  [1]=>
  array(1) {
    ["key3"]=>
    int(3)
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(3)
  }
}

-- Iteration 4 --
array(2) {
  [0]=>
  array(2) {
    [1]=>
    string(3) "one"
    [2]=>
    string(3) "two"
  }
  [1]=>
  array(1) {
    [3]=>
    string(5) "three"
  }
}
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "one"
    [1]=>
    string(3) "two"
  }
  [1]=>
  array(1) {
    [0]=>
    string(5) "three"
  }
}

-- Iteration 5 --
array(3) {
  [0]=>
  array(2) {
    [1]=>
    string(3) "one"
    [2]=>
    string(3) "two"
  }
  [1]=>
  array(2) {
    [3]=>
    string(5) "three"
    [4]=>
    int(4)
  }
  [2]=>
  array(1) {
    ["five"]=>
    int(5)
  }
}
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "one"
    [1]=>
    string(3) "two"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "three"
    [1]=>
    int(4)
  }
  [2]=>
  array(1) {
    [0]=>
    int(5)
  }
}
Done
