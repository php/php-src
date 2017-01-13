--TEST--
Test error operation of random_int()
--FILE--
<?php
//-=-=-=-

try {
    $randomInt = random_int();
} catch (TypeError $e) {
    echo $e->getMessage().PHP_EOL;
}

try {
    $randomInt = random_int(42);
} catch (TypeError $e) {
    echo $e->getMessage().PHP_EOL;
}

try {
    $randomInt = random_int(42,0);
} catch (Error $e) {
    echo $e->getMessage().PHP_EOL;
}

?>
--EXPECT--
random_int() expects exactly 2 parameters, 0 given
random_int() expects exactly 2 parameters, 1 given
Minimum value must be less than or equal to the maximum value
