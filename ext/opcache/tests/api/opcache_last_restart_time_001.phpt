--TEST--
opcache_last_restart_time() api 001: agrees with opcache_get_status()
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
zend_test
--FILE--
<?php

$status = opcache_get_status(false);
$expected = $status['opcache_statistics']['last_restart_time'] ?? 0;

var_dump(zend_test_opcache_last_restart_time() === $expected);

?>
--EXPECT--
bool(true)
