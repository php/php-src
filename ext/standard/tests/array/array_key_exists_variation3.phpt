--TEST--
Test array_key_exists() function : usage variations - floats and casting to ints
--FILE--
<?php
/*
 * Pass floats as $key argument, then cast float values
 * to integers and pass as $key argument
 */

echo "*** Testing array_key_exists() : usage variations ***\n";

$keys = array(1.2345678900E-10, 1.00000000000001, 1.99999999999999);

$search = array ('zero', 'one', 'two');

$iterator = 1;
foreach($keys as $key) {
    echo "\n-- Iteration $iterator --\n";
    echo "Pass float as \$key:\n";
    try {
        var_dump(array_key_exists($key, $search));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
    echo "Cast float to int:\n";
    var_dump(array_key_exists((int)$key, $search));
}

echo "Done";
?>
--EXPECTF--
*** Testing array_key_exists() : usage variations ***

-- Iteration 1 --
Pass float as $key:

Deprecated: Implicit conversion from float 1.23456789E-10 to int loses precision in %s on line %d
bool(true)
Cast float to int:
bool(true)

-- Iteration 1 --
Pass float as $key:

Deprecated: Implicit conversion from float 1.00000000000001 to int loses precision in %s on line %d
bool(true)
Cast float to int:
bool(true)

-- Iteration 1 --
Pass float as $key:

Deprecated: Implicit conversion from float 1.99999999999999 to int loses precision in %s on line %d
bool(true)
Cast float to int:
bool(true)
Done
