--TEST--
Test array_multisort() function : error conditions
--FILE--
<?php
echo "*** Testing array_multisort() : error conditions ***\n";

echo "\n-- Testing array_multisort() function with repeated flags --\n";
$ar1 = array(1);
try {
    var_dump( array_multisort($ar1, SORT_ASC, SORT_ASC) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing array_multisort() function with repeated flags --\n";
$ar1 = array(1);
try {
    var_dump( array_multisort($ar1, SORT_STRING, SORT_NUMERIC) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing array_multisort() : error conditions ***

-- Testing array_multisort() function with repeated flags --
array_multisort(): Argument #3 must be an array or a sort flag that has not already been specified

-- Testing array_multisort() function with repeated flags --
array_multisort(): Argument #3 must be an array or a sort flag that has not already been specified
