--TEST--
Test error operation of random_bytes()
--FILE--
<?php
//-=-=-=-
var_dump(random_bytes(0));
try {
    $bytes = random_bytes();
} catch (TypeError $e) {
    echo $e->getMessage().PHP_EOL;
}

try {
    $bytes = random_bytes(-1);
} catch (Error $e) {
    echo $e->getMessage().PHP_EOL;
}

?>
--EXPECT--
string(0) ""
random_bytes() expects exactly 1 argument, 0 given
random_bytes(): Argument #1 ($length) must be greater than or equal to 0
