--TEST--
Test array_pad() function : usage variations - two dimensional array for 'pad_value' argument
--FILE--
<?php
/*
* Passing two dimensional array to $pad_value argument and testing whether
* array_pad() behaves in an expected way with the other arguments passed to the function.
* The $input and $pad_size arguments passed are fixed values.
*/

echo "*** Testing array_pad() : Passing 2-d array to \$pad_value argument ***\n";

// initialize the $input and $pad_size argument
$input = array(1, 2, 3);
$pad_size = 5;

// initialize $pad_value
$pad_value = array (
  array(1),
  array("hello", 'world'),
  array("one" => 1, 'two' => 2)
);

var_dump( array_pad($input, $pad_size, $pad_value) );  // positive 'pad_value'
var_dump( array_pad($input, -$pad_size, $pad_value) );  // negative 'pad_value'

echo "Done";
?>
--EXPECT--
*** Testing array_pad() : Passing 2-d array to $pad_value argument ***
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(3) {
    [0]=>
    array(1) {
      [0]=>
      int(1)
    }
    [1]=>
    array(2) {
      [0]=>
      string(5) "hello"
      [1]=>
      string(5) "world"
    }
    [2]=>
    array(2) {
      ["one"]=>
      int(1)
      ["two"]=>
      int(2)
    }
  }
  [4]=>
  array(3) {
    [0]=>
    array(1) {
      [0]=>
      int(1)
    }
    [1]=>
    array(2) {
      [0]=>
      string(5) "hello"
      [1]=>
      string(5) "world"
    }
    [2]=>
    array(2) {
      ["one"]=>
      int(1)
      ["two"]=>
      int(2)
    }
  }
}
array(5) {
  [0]=>
  array(3) {
    [0]=>
    array(1) {
      [0]=>
      int(1)
    }
    [1]=>
    array(2) {
      [0]=>
      string(5) "hello"
      [1]=>
      string(5) "world"
    }
    [2]=>
    array(2) {
      ["one"]=>
      int(1)
      ["two"]=>
      int(2)
    }
  }
  [1]=>
  array(3) {
    [0]=>
    array(1) {
      [0]=>
      int(1)
    }
    [1]=>
    array(2) {
      [0]=>
      string(5) "hello"
      [1]=>
      string(5) "world"
    }
    [2]=>
    array(2) {
      ["one"]=>
      int(1)
      ["two"]=>
      int(2)
    }
  }
  [2]=>
  int(1)
  [3]=>
  int(2)
  [4]=>
  int(3)
}
Done
