--TEST--
dns_get_record() error conditions
--FILE--
<?php
try {
    // A random DNS Mode
    dns_get_record('php.net', 15263480);
} catch (\ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    // DNS Mode 0
    $auth = [];
    $additional = [];
    dns_get_record('php.net', 0, $auth, $additional, true);
} catch (\ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    // A random DNS Mode
    $auth = [];
    $additional = [];
    dns_get_record('php.net', 15263480, $auth, $additional, true);
} catch (\ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
dns_get_record(): Argument #2 ($type) must be a DNS_* constant
dns_get_record(): Argument #2 ($type) must be between 1 and 65535 when argument #5 ($raw) is true
dns_get_record(): Argument #2 ($type) must be between 1 and 65535 when argument #5 ($raw) is true
