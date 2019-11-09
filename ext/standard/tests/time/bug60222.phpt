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
--EXPECTF--
The seconds value must be greater than 0
The nanoseconds value must be greater than 0
