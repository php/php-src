--TEST--
CLI: stderr is available in mshutdown
--XFAIL--
GH-8952 / GH-8833
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
[zend-test] MSHUTDOWN
