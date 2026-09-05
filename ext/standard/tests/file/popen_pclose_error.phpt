--TEST--
Test popen() and pclose function: error conditions
--FILE--
<?php

try {
    popen("abc.txt", "x");
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    popen("abc.txt", "rw");
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    popen("abc.txt", "rwb");
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: popen(): Argument #2 ($mode) must be one of "r", "rb", "w", or "wb"
ValueError: popen(): Argument #2 ($mode) must be one of "r", "rb", "w", or "wb"
ValueError: popen(): Argument #2 ($mode) must be one of "r", "rb", "w", or "wb"
