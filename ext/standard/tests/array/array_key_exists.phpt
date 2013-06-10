--TEST--
Test array_key_exists() function
--FILE--
<?php
/* Prototype: 
 *  bool array_key_exists ( mixed $key, array $search );
 * Description:
 *  Returns TRUE if the given key is set in the array. 
 *  key can be any value possible for an array index. 
 *  Also also works on objects.
 */ 

echo "*** Testing basic functionalities ***\n";
/* Arrays with regular values */
$search_arrays = array(
           array(1,2,3,4),
           array('a','b','c'),
           array('abc', 'bcd', 'dcf'),
           array("test", "rest", "enjoy"),
           array("Name" => "Jack", "Loc" => "Mars", "Id" => "MS123"),
           array('Red' => 'Rose', 'I' => 'You'),
           array(0 => 'Zero', 1 => 'One', 2 => 'Two', 3 => "Three" ),
           array(0.1 => 'Zero', 1.1 => 'One', 2.2 => 'Two', 3.3 => "Three" )
          );
/* keys to search in $search_arrays. $keys[0] 
   is the key to be searched in $search_arrays[0] and so on */
$keys = array( 1, 'a', 2, 4, "Name", "Red", 0, 3 ); 

$key_counter = 0;
foreach ($search_arrays as $search_array) {
  $key = $keys[ $key_counter++ ];
  echo "-- Iteration $key_counter --\n";
  var_dump( array_key_exists($key, $search_array) );
}

echo "\n*** Testing possible variations ***\n";
// use different keys on each sub array of  $search_arrays
$key_variations = array ("", NULL, null, " ", '', "test", 1);
$key_counter = 0;
$key_count = count ( $key_variations );
echo "\n** Variation loop 1 **\n";
$out_loop_count = 0;
foreach ($search_arrays as $search_array) {
  $key_counter = 0;
  $out_loop_count ++; echo "-- Iteration $out_loop_count --\n";
  while ( $key_counter < $key_count ) {
    $key = $key_variations[ $key_counter++ ];
    var_dump( array_key_exists($key, $search_array) );
  }
}
// arrays with variation in elements  
$search_arrays_v = array (
                     array(),
                     array(NULL),
                     array(array(), 1, 2),
                     array(1,2,3, "" => "value", NULL => "value", true => "value" ),
                     array( array(2,4,5), array ("a","b","d") )
                   );
// search for $key_variations in each sub array of $search_arrays_v
echo "\n** Variation loop 2 **\n";
$out_loop_count = 0;
foreach ($search_arrays_v as $search_array) {
  $key_counter = 0;
  $out_loop_count ++; echo "-- Iteration $out_loop_count --\n";
  while ( $key_counter < $key_count ) {
    $key = $key_variations[ $key_counter++ ];
    var_dump( array_key_exists($key, $search_array) );
  }
}

echo "\n*** Testing error conditions ***\n";
//Zeor args
var_dump( array_key_exists() );
// first args as array 
var_dump( array_key_exists(array(), array()) );
// second args as string
var_dump( array_key_exists("", "") ); 
// second args a integer
var_dump( array_key_exists(1, 1) ); 
// second args as NULL
var_dump( array_key_exists(1, NULL) ); 
// second args as boolean
var_dump( array_key_exists(1, true) ); 
// first args as boolean
var_dump( array_key_exists(false, true) ); 
// second args as float 
var_dump( array_key_exists(false, 17.5) ); 
// args more than expected 
var_dump( array_key_exists(1, array(), array()) ); 
// first argument as floating point value
var_dump( array_key_exists(17.5, array(1,23) ) ) ; 

echo "\n*** Testing operation on objects ***\n";
class key_check
{
  private   $private_var = "Priviate var";
  protected $protected_var = "Protected var";
  public    $public_var = "Public var";
  public    $arr = array("var" => "value", "1" => "one", ""=>"value");
  public function print_member()
  {
    echo $this->$private_var."\n";
    echo $this->$protected_var."\n";
    echo $this->$public_var."\n";
  }
}

$key_check_obj = new key_check; //new object
/* array_key_exists() on an object, it should work on only public member variables */
var_dump(array_key_exists("private_var", $key_check_obj)); // not found, private member
var_dump(array_key_exists("protected_var", $key_check_obj)); // not found, private member
var_dump(array_key_exists("public_var", $key_check_obj)); // found, public member
var_dump(array_key_exists("print_member", $key_check_obj)); // not found, its a function
var_dump(array_key_exists("arr", $key_check_obj)); //found, public member
var_dump(array_key_exists("var", $key_check_obj->arr)); //found,  key is in member array

/* error condition, first argument as object */
var_dump( array_key_exists($key_check_obj, $key_check_obj) );
echo "Done\n";
?>
--EXPECTF--	
*** Testing basic functionalities ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
bool(true)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)

*** Testing possible variations ***

** Variation loop 1 **
-- Iteration 1 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
-- Iteration 2 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
-- Iteration 3 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
-- Iteration 4 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
-- Iteration 5 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 7 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
-- Iteration 8 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)

** Variation loop 2 **
-- Iteration 1 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 2 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
-- Iteration 3 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
-- Iteration 4 --
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
-- Iteration 5 --
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)

*** Testing error conditions ***

Warning: array_key_exists() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

Warning: array_key_exists() expects parameter 2 to be array, string given in %s on line %d
NULL

Warning: array_key_exists() expects parameter 2 to be array, integer given in %s on line %d
NULL

Warning: array_key_exists() expects parameter 2 to be array, null given in %s on line %d
NULL

Warning: array_key_exists() expects parameter 2 to be array, boolean given in %s on line %d
NULL

Warning: array_key_exists() expects parameter 2 to be array, boolean given in %s on line %d
NULL

Warning: array_key_exists() expects parameter 2 to be array, double given in %s on line %d
NULL

Warning: array_key_exists() expects exactly 2 parameters, 3 given in %s on line %d
NULL

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)

*** Testing operation on objects ***
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)

Warning: array_key_exists(): The first argument should be either a string or an integer in %s on line %d
bool(false)
Done
