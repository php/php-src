--TEST--
Test property_exists() function : error conditions
--FILE--
<?php
echo "*** Testing property_exists() : error conditions ***\n";

echo "\n-- Testing property_exists() function with incorrect arguments --\n";
$property_name = 'string_val';

try {
    var_dump( property_exists(10, $property_name) );
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing property_exists() : error conditions ***

-- Testing property_exists() function with incorrect arguments --
property_exists(): Argument #1 ($object_or_class) must be of type object|string, int given
