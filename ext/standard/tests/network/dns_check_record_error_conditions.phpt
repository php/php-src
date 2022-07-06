--TEST--
dns_check_record() error conditions
--FILE--
<?php
try {
    dns_check_record('');
} catch (\ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    // A random DNS Mode
    dns_check_record('php.net', 15263480);
} catch (\ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
dns_check_record(): Argument #1 ($hostname) cannot be empty
dns_check_record(): Argument #2 ($type) must be a valid DNS record type
