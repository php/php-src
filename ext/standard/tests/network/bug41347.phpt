--TEST--
dns_check_record() segfault with empty host
--FILE--
<?php
try {
    var_dump(dns_check_record(''));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
dns_check_record(): Argument #1 ($hostname) cannot be empty
