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
random_int() expects exactly 2 arguments, 0 given
random_int() expects exactly 2 arguments, 1 given
random_int(): Argument #1 ($min) must be less than or equal to argument #2 ($max)
