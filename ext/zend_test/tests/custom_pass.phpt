--TEST--
Test custom pass
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
zend_test.open_custom_pass=1
--FILE--
<?php
?>
--EXPECT--
pass1
pass2
