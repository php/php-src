--TEST--
Test log() - wrong params test log()
--INI--
precision=14
--FILE--
<?php
try {
    log(36, -4);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    log(36, 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
log(): Argument #2 ($base) must not be 1 or less than or equal to 0
log(): Argument #2 ($base) must not be 1 or less than or equal to 0
