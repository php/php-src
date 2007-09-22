--TEST--
Test in_array() function : usage variations - haystack as sub-array/object
--FILE--
<?php
/*
 * Prototype  : bool in_array ( mixed $needle, array $haystack [, bool $strict] )
 * Description: Searches haystack for needle and returns TRUE  
 *              if it is found in the array, FALSE otherwise.
 * Source Code: ext/standard/array.c
*/

/* Test in_array() with haystack as sub-array and object */

/* checking for sub-arrays with in_array() */
echo "*** Testing sub-arrays with in_array() ***\n";
$sub_array = array (
  "one", 
  array(1, 2 => "two", "three" => 3),
  4 => "four",
  "five" => 5,
  array('', 'i')
);
var_dump( in_array("four", $sub_array) );
//checking for element in a sub-array
var_dump( in_array(3, $sub_array[1]) ); 
var_dump( in_array(array('','i'), $sub_array) );

/* checking for objects in in_array() */
echo "\n*** Testing objects with in_array() ***\n";
class in_array_check {
  public $array_var = array(1=>"one", "two"=>2, 3=>3);
  public function foo() {
    echo "Public function\n";
  }
}

$in_array_obj = new in_array_check();  //creating new object
//error: as wrong datatype for second argument
var_dump( in_array("array_var", $in_array_obj) ); 
//error: as wrong datatype for second argument
var_dump( in_array("foo", $in_array_obj) ); 
//element found as "one" exists in array $array_var
var_dump( in_array("one", $in_array_obj->array_var) ); 

echo "Done\n";
?>
--EXPECTF--
*** Testing sub-arrays with in_array() ***
bool(true)
bool(true)
bool(true)

*** Testing objects with in_array() ***

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)

Warning: in_array(): Wrong datatype for second argument in %s on line %d
bool(false)
bool(true)
Done
