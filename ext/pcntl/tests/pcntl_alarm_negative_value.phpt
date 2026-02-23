--TEST--
pcntl_alarm() rejects negative values
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

var_dump(pcntl_alarm(0));

?>
--EXPECT--
pcntl_alarm(): Argument #1 ($seconds) must be greater or equal to 0
pcntl_alarm(): Argument #1 ($seconds) must be greater or equal to 0
int(0)
