--TEST--
Test suppressing arginfo / zpp mismatch
--EXTENSIONS--
zend_test
--SKIPIF--
<?php if (!PHP_DEBUG) die('skip debug build only'); ?>
--ENV--
ZEND_SUPPRESS_ARGINFO_ZPP_MISMATCH=1
--FILE--
<?php
zend_test_arginfo_zpp_mismatch(1);
echo 'success';
--EXPECT--
success
