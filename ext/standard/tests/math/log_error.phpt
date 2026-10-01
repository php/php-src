--TEST--
Test log() - wrong params test log()
--INI--
precision=14
--FILE--
<?php
try {
    log(36, -4);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: log(): Argument #2 ($base) must be greater than 0
