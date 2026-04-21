--TEST--
getenv() rejects null bytes
--FILE--
<?php

foreach ([false, true] as $local_only) {
    try {
        getenv("PHP_GETENV_NUL_TEST\0SUFFIX", $local_only);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

?>
--EXPECT--
getenv(): Argument #1 ($name) must not contain any null bytes
getenv(): Argument #1 ($name) must not contain any null bytes
