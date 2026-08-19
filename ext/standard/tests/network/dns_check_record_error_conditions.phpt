--TEST--
dns_check_record() error conditions
--FILE--
<?php
try {
    dns_check_record('');
} catch (\Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
try {
    // A random DNS Mode
    dns_check_record('php.net', 15263480);
} catch (\Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: dns_check_record(): Argument #1 ($hostname) must not be empty
ValueError: dns_check_record(): Argument #2 ($type) must be a valid DNS record type
