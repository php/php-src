--TEST--
Test array_map() function : error conditions
--FILE--
<?php
echo "*** Testing array_map() : error conditions ***\n";

// Testing array_map with one less than the expected number of arguments
echo "\n-- Testing array_map() function with one less than expected no. of arguments --\n";
function callback1() {
  return 1;
}
try {
    var_dump( array_map('callback1') );
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "\n-- Testing array_map() function with less no. of arrays than callback function arguments --\n";
$arr1 = array(1, 2);
function callback2($p, $q) {
  return $p * $q;
}
try {
    var_dump( array_map('callback2', $arr1) );
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

echo "\n-- Testing array_map() function with more no. of arrays than callback function arguments --\n";
$arr2 = array(3, 4);
$arr3 = array(5, 6);
var_dump( array_map('callback2', $arr1, $arr2, $arr3) );

echo "Done";
?>
--EXPECT--
*** Testing array_map() : error conditions ***

-- Testing array_map() function with one less than expected no. of arguments --
Exception: array_map() expects at least 2 arguments, 1 given

-- Testing array_map() function with less no. of arrays than callback function arguments --
Exception: Too few arguments to function callback2(), 1 passed and exactly 2 expected

-- Testing array_map() function with more no. of arrays than callback function arguments --
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(8)
}
Done
