--TEST--
fsockopen() and pfsockopen(): ValueError for $timeout reports correct argument number and name
--FILE--
<?php
try {
    fsockopen('localhost', 80, $errno, $errstr, -2.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    pfsockopen('localhost', 80, $errno, $errstr, -2.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
ValueError: fsockopen(): Argument #5 ($timeout) must be -1 or between 0 and %s
ValueError: pfsockopen(): Argument #5 ($timeout) must be -1 or between 0 and %s
