--TEST--
Test don't suppress arginfo / zpp mismatch
--EXTENSIONS--
zend_test
--SKIPIF--
<?php if (!PHP_DEBUG) die('skip debug build only'); ?>
--FILE--
<?php
zend_test_arginfo_zpp_mismatch(1);
echo 'success';
--EXPECTF--
Fatal error: Arginfo / zpp mismatch during call of zend_test_arginfo_zpp_mismatch() in %s on line %d
