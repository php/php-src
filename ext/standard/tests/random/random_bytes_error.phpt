--TEST--
Test error operation of random_bytes()
--FILE--
<?php
//-=-=-=-

try {
    $bytes = random_bytes();
} catch (TypeError $e) {
    echo $e->getMessage().PHP_EOL;
}

try {
    $bytes = random_bytes(0);
} catch (Error $e) {
    echo $e->getMessage().PHP_EOL;
}

?>
--EXPECT--
random_bytes() expects exactly 1 parameter, 0 given
Length must be greater than 0
