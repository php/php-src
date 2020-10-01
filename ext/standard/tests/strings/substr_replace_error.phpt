--TEST--
Test substr_replace() function : error conditions
--FILE--
<?php
/*
 * Testing substr_replace() for error conditions
*/

echo "*** Testing substr_replace() : error conditions ***\n";

$s1 = "Good morning";

echo "\n-- Testing substr_replace() function with start and length as arrays but string not--\n";
try {
    var_dump(substr_replace($s1, "evening", array(5)));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(substr_replace($s1, "evening", 5, array(1)));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
*** Testing substr_replace() : error conditions ***

-- Testing substr_replace() function with start and length as arrays but string not--
substr_replace(): Argument #3 ($offset) cannot be an array when working on a single string
substr_replace(): Argument #4 ($length) cannot be an array when working on a single string
