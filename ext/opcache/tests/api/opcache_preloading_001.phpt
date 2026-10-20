--TEST--
opcache_preloading() api 001
--EXTENSIONS--
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/opcache_preloading.inc
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
?>
--FILE--
<?php

printf("%s: %d\n", __FILE__, zend_test_opcache_preloading());

?>
--EXPECTF--
%sopcache_preloading.inc: 1
%sopcache_preloading_001.php: 0
