--TEST--
Test array_diff_key() function : usage variation - Passing multi-dimensional array
--FILE--
<?php
echo "*** Testing array_diff_key() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$array1 = array(

      'first' => array('blue' => 1, 'red' => 2),

      'second' => array('yellow' => 7),

      'third' => array(0 => 'zero'),
);

$array2 = array (

      'first' => array('blue' => 1, 'red' => 2,),

      'second' => array('cyan' => 8),

      'fourth' => array(2 => 'two'),
);

echo "\n-- Testing array_diff_key() function with multi dimensional array --\n";
var_dump( array_diff_key($array1, $array2) );
var_dump( array_diff_key($array2, $array1) );
?>
--EXPECT--
*** Testing array_diff_key() : usage variation ***

-- Testing array_diff_key() function with multi dimensional array --
array(1) {
  ["third"]=>
  array(1) {
    [0]=>
    string(4) "zero"
  }
}
array(1) {
  ["fourth"]=>
  array(1) {
    [2]=>
    string(3) "two"
  }
}
