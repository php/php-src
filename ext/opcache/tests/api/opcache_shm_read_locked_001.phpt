--TEST--
opcache_shm_read_locked() api 001: the lock is held once the script was loaded through the cache
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
zend_test
--FILE--
<?php

// Asked first: opcache_get_status() takes the lock itself if it is not held.
$locked = zend_test_opcache_shm_read_locked();
$status = opcache_get_status(false);

var_dump($locked === ($status['opcache_enabled'] ?? false));

?>
--EXPECT--
bool(true)
