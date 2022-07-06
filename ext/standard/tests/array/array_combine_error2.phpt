--TEST--
Test array_combine() function : error conditions - empty array
--FILE--
<?php
echo "*** Testing array_combine() : error conditions specific to array_combine() ***\n";

// Testing array_combine by passing empty arrays to $keys and $values arguments
echo "\n-- Testing array_combine() function with empty arrays --\n";
var_dump( array_combine(array(), array()) );

// Testing array_combine by passing empty array to $keys
echo "\n-- Testing array_combine() function with empty array for \$keys argument --\n";
try {
    var_dump( array_combine(array(), array(1, 2)) );
} catch (\ValueError $e) {
    echo $e->getMessage();
}

// Testing array_combine by passing empty array to $values
echo "\n-- Testing array_combine() function with empty array for \$values argument --\n";
try {
    var_dump( array_combine(array(1, 2), array()) );
} catch (\ValueError $e) {
    echo $e->getMessage();
}

// Testing array_combine with arrays having unequal number of elements
echo "\n-- Testing array_combine() function by passing array with unequal number of elements --\n";
try {
    var_dump( array_combine(array(1, 2), array(1, 2, 3)) );
} catch (\ValueError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
*** Testing array_combine() : error conditions specific to array_combine() ***

-- Testing array_combine() function with empty arrays --
array(0) {
}

-- Testing array_combine() function with empty array for $keys argument --
array_combine(): Argument #1 ($keys) and argument #2 ($values) must have the same number of elements
-- Testing array_combine() function with empty array for $values argument --
array_combine(): Argument #1 ($keys) and argument #2 ($values) must have the same number of elements
-- Testing array_combine() function by passing array with unequal number of elements --
array_combine(): Argument #1 ($keys) and argument #2 ($values) must have the same number of elements
