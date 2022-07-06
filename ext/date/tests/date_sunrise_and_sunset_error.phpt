--TEST--
Test error condition of date_sunrise() and date_sunset()
--FILE--
<?php

try {
    date_sunrise(time(), 3);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    date_sunset(time(), 4);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
date_sunrise(): Argument #2 ($returnFormat) must be one of SUNFUNCS_RET_TIMESTAMP, SUNFUNCS_RET_STRING, or SUNFUNCS_RET_DOUBLE
date_sunset(): Argument #2 ($returnFormat) must be one of SUNFUNCS_RET_TIMESTAMP, SUNFUNCS_RET_STRING, or SUNFUNCS_RET_DOUBLE
