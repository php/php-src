--TEST--
Test array_search() function : usage variations - haystack as sub-array/object
--FILE--
<?php
/*
 * Prototype  : mixed array_search ( mixed $needle, array $haystack [, bool $strict] )
 * Description: Searches haystack for needle and returns the key if it is found in the array, FALSE otherwise
 * Source Code: ext/standard/array.c
*/

/* checking for sub-arrays with array_search() */
echo "*** Testing sub-arrays with array_search() ***\n";
$sub_array = array (
  "one", 
  array(1, 2 => "two", "three" => 3),
  4 => "four",
  "five" => 5,
  array('', 'i')
);
var_dump( array_search("four", $sub_array) );
//checking for element in a sub-array
var_dump( array_search(3, $sub_array[1]) ); 
var_dump( array_search(array('','i'), $sub_array) );

/* checking for objects in array_search() */
echo "\n*** Testing objects with array_search() ***\n";
class array_search_check {
  public $array_var = array(1=>"one", "two"=>2, 3=>3);
  public function foo() {
    echo "Public function\n";
  }
}

$array_search_obj = new array_search_check();  //creating new object
//error: as wrong datatype for second argument
var_dump( array_search("array_var", $array_search_obj) ); 
//error: as wrong datatype for second argument
var_dump( array_search("foo", $array_search_obj) ); 
//element found as "one" exists in array $array_var
var_dump( array_search("one", $array_search_obj->array_var) ); 

echo "Done\n";
?>
--EXPECTF--
*** Testing sub-arrays with array_search() ***
int(4)
string(5) "three"
int(5)

*** Testing objects with array_search() ***

Warning: array_search(): Wrong datatype for second argument in %s on line %d
bool(false)

Warning: array_search(): Wrong datatype for second argument in %s on line %d
bool(false)
int(1)
Done
