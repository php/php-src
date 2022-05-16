--TEST--
CLI: stderr is available in mshutdown
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.print_stderr_mshutdown=1
--FILE--
==DONE==
--EXPECTF--
==DONE==
[zend-test] MSHUTDOWN
