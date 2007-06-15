--TEST--
dns_check_record() segfault with empty host
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip No windows support');
}
?>
--FILE--
<?php
var_dump(dns_check_record(''));
?>
--EXPECTF--
Warning: dns_check_record(): Host cannot be empty in %s on line %d
bool(false)
