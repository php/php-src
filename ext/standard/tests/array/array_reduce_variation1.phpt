--TEST--
Test array_reduce() function : variation
--FILE--
<?php
echo "*** Testing array_reduce() : variation ***\n";


function oneArg($v) {
  return $v;
}

function threeArgs($v, $w, $x) {
  return $v + $w + $x;
}

$array = array(1);

echo "\n--- Testing with a callback with too few parameters ---\n";
var_dump(array_reduce($array, "oneArg", 2));

echo "\n--- Testing with a callback with too many parameters ---\n";
try {
    var_dump(array_reduce($array, "threeArgs", 2));
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing array_reduce() : variation ***

--- Testing with a callback with too few parameters ---
int(2)

--- Testing with a callback with too many parameters ---
Exception: Too few arguments to function threeArgs(), 2 passed and exactly 3 expected
