--TEST--
log(): Throw ValueError for invalid base values
--FILE--
<?php

try {
    log(10, 1);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    log(10, 0);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    log(10, -2);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

echo log(8, 2), PHP_EOL;
echo log(100, 10), PHP_EOL;

?>
--EXPECT--
log(): Argument #2 ($base) must not be equal to 1
log(): Argument #2 ($base) must be greater than 0
log(): Argument #2 ($base) must be greater than 0
3
2
