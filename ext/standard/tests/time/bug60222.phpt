--TEST--
Bug #60222 (time_nanosleep() does validate input params)
--FILE--
<?php
    try {
        time_nanosleep(-1, 0);
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }

    try {
        time_nanosleep(0, -1);
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
?>
--EXPECT--
ValueError: time_nanosleep(): Argument #1 ($seconds) must be greater than or equal to 0
ValueError: time_nanosleep(): Argument #2 ($nanoseconds) must be greater than or equal to 0
