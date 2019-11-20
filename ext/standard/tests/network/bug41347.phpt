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
Host cannot be empty
