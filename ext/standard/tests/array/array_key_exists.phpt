--TEST--
Test array_key_exists() function
--FILE--
<?php
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
// first args as array
try {
    array_key_exists(array(), array());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "\n*** Testing operation on objects ***\n";
class key_check
{
  public $public_var = "Public var";
}

$key_check_obj = new key_check; //new object
try {
    var_dump(array_key_exists("public_var", $key_check_obj));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECT--
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
Cannot access offset of type array on array

*** Testing operation on objects ***
array_key_exists(): Argument #2 ($array) must be of type array, key_check given
Done
