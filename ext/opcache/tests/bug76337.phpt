--TEST--
Bug 76337: segmentation fault when an extension use zend_register_class_alias() and opcache enabled
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!extension_loaded('zend-test')) die('skip zend-test extension not loaded');
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
var_dump(class_exists('_ZendTestClassAlias'));
?>
--EXPECT--
bool(true)
