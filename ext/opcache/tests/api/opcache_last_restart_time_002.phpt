--TEST--
opcache_last_restart_time() api 002: OPcache is not started
--INI--
opcache.enable=1
opcache.enable_cli=0
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_opcache_last_restart_time());

?>
--EXPECT--
int(0)
