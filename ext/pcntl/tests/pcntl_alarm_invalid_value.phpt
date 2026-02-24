--TEST--
pcntl_alarm() rejects invalid values
--EXTENSIONS--
pcntl
--FILE--
<?php

try {
    pcntl_alarm(-1);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    pcntl_alarm(PHP_INT_MIN);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    pcntl_alarm(PHP_INT_MAX);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(pcntl_alarm(0));

?>
--EXPECTF--
pcntl_alarm(): Argument #1 ($seconds) must be between 0 and %d
pcntl_alarm(): Argument #1 ($seconds) must be between 0 and %d
pcntl_alarm(): Argument #1 ($seconds) must be between 0 and %d
int(0)
