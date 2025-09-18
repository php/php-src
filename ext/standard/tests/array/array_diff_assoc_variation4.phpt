--TEST--
Test array_diff_assoc() function : usage variations - arrays with different data types as keys
--FILE--
<?php
/*
 * Test how array_diff_assoc() compares arrays containing different data types
 * as keys
 */

echo "\n*** Testing array_diff_assoc() : usage variations ***\n";

$array = array(1, 2, 3);

//Different data types as keys to be passed to $arr1 argument
$inputs = array(

       // int data
/*1*/
'int' => array(
       0 => 'zero',
       1 => 'one',
       12345 => 'positive',
       -2345 => 'negative'),

       // empty data
/*5*/
'empty' => array(
      '' => 'empty'),

       // string data
/*6*/
'string' => array(
      'string' => 'strings'),

);

// loop through each element of $inputs to check the behavior of array_diff_assoc
$iterator = 1;
foreach($inputs as $key => $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_diff_assoc($input, $array));
  $iterator++;
};

echo "Done";
?>
--EXPECT--
*** Testing array_diff_assoc() : usage variations ***

-- Iteration 1 --
array(4) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [12345]=>
  string(8) "positive"
  [-2345]=>
  string(8) "negative"
}

-- Iteration 2 --
array(1) {
  [""]=>
  string(5) "empty"
}

-- Iteration 3 --
array(1) {
  ["string"]=>
  string(7) "strings"
}
Done
