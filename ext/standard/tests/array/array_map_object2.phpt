--TEST--
Test array_map() function : object functionality - with non-existent class and method
--FILE--
<?php
/* Prototype  : array array_map  ( callback $callback  , array $arr1  [, array $...  ] )
 * Description: Applies the callback to the elements of the given arrays
 * Source code: ext/standard/array.c
 */

/*
 * Testing array_map() for following object functionalities:
 *   1) non-existent class
 *   2) existent class and non-existent function
 */
echo "*** Testing array_map() :  with non-existent class and method ***\n";

class SimpleClass
{
  public $var1 = 1;
  public function square($n) {
    return $n * $n;
  }
  public static function cube($n) {
    return $n * $n * $n;
  }
}

echo "-- with non-existent class --\n";
try {
    var_dump( array_map(array('non-existent', 'square'), array(1, 2)) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "-- with existent class and non-existent method --\n";
try {
    var_dump( array_map(array('SimpleClass', 'non-existent'), array(1, 2)) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_map() :  with non-existent class and method ***
-- with non-existent class --
array_map(): Argument #1 ($callback) must be a valid callback, class 'non-existent' not found
-- with existent class and non-existent method --
array_map(): Argument #1 ($callback) must be a valid callback, class 'SimpleClass' does not have a method 'non-existent'
Done
