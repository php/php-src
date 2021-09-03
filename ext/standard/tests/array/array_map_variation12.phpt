--TEST--
Test array_map() function : usage variations - built-in function as callback
--FILE--
<?php
/*
 * Test array_map() by passing buit-in function as callback function
 */

echo "*** Testing array_map() : built-in function ***\n";

$array1 = array(1, 2, 3);
$array2 = array(3, 4, 5);

echo "-- with built-in function 'pow' and two parameters --\n";
var_dump( array_map('pow', $array1, $array2));

echo "-- with built-in function 'pow' and one parameter --\n";
try {
    var_dump( array_map('pow', $array1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "-- with language construct --\n";
try {
    var_dump( array_map('echo', $array1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done";
?>
--EXPECT--
*** Testing array_map() : built-in function ***
-- with built-in function 'pow' and two parameters --
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(16)
  [2]=>
  int(243)
}
-- with built-in function 'pow' and one parameter --
pow() expects exactly 2 arguments, 1 given
-- with language construct --
array_map(): Argument #1 ($callback) must be a valid callback or null, function "echo" not found or invalid function name
Done
