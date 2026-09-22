--TEST--
opcache_preloading() api 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/opcache_preloading.inc
opcache.preload_user={ENV:TEST_NON_ROOT_USER}
--EXTENSIONS--
posix
zend_test
--SKIPIF--
<?php
if (PHP_OS_FAMILY == 'Windows') die('skip Preloading is not supported on Windows');
if (posix_geteuid() !== 0) die('skip Test needs root user');
?>
--FILE--
<?php

printf("%s: %d\n", __FILE__, zend_test_opcache_preloading());

?>
--EXPECTF--
%sopcache_preloading.inc: 1
%sopcache_preloading_002.php: 0
