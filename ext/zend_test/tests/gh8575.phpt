--TEST--
CLI: stderr is available in mshutdown
--SKIPIF--
<?php if (php_sapi_name() != "cli") die('skip cli test only'); ?>
--EXTENSIONS--
zend_test
--INI--
zend_test.print_stderr_mshutdown=1
--FILE--
==DONE==
--EXPECTF--
==DONE==
[zend_test] MSHUTDOWN
