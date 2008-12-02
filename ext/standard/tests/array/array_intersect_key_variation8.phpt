--TEST--
Test array_intersect_key() function : usage variation - Passing Multi dimensional array
--FILE--
<?php
/* Prototype  : array array_intersect_key(array arr1, array arr2 [, array ...])
 * Description: Returns the entries of arr1 that have keys which are present in all the other arguments. 
 * Source code: ext/standard/array.c
 */

echo "*** Testing array_intersect_key() : usage variation ***\n";

/// Initialise function arguments not being substituted (if any)
$array1 = array(

	  'first' => array('blue'  => 1, 'red'  => 2),
              
      'second' => array('yellow' => 7),
              
      'third' => array(0 =>'zero'),
);

$array2 = array (

	  'first' => array('blue'  => 1, 'red'  => 2,),
              
      'second' => array('cyan'   => 8),
              
      'fourth' => array(2 => 'two'), 
);
var_dump( array_intersect_key($array1, $array2) );
var_dump( array_intersect_key($array2,$array1 ) ); 
?>
===DONE===
--EXPECTF--
*** Testing array_intersect_key() : usage variation ***
array(2) {
  ["first"]=>
  array(2) {
    ["blue"]=>
    int(1)
    ["red"]=>
    int(2)
  }
  ["second"]=>
  array(1) {
    ["yellow"]=>
    int(7)
  }
}
array(2) {
  ["first"]=>
  array(2) {
    ["blue"]=>
    int(1)
    ["red"]=>
    int(2)
  }
  ["second"]=>
  array(1) {
    ["cyan"]=>
    int(8)
  }
}
===DONE===
