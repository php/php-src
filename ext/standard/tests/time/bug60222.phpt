--TEST--
Bug #60222 (time_nanosleep() does validate input params)
--FILE--
<?php
    try {
        time_nanosleep(-1, 0);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }

    try {
        time_nanosleep(0, -1);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
?>
--EXPECT--
time_nanosleep(): Argument #1 ($seconds) must be greater than or equal to 0
time_nanosleep(): Argument #2 ($nanoseconds) must be greater than or equal to 0
