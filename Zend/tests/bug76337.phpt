--TEST--
Bug 76337: segmentation fault when an extension use zend_register_class_alias() and opcache enabled
--SKIPIF--
<?php
if (!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not loaded');
if (!class_exists('_ZendTestClass')) die('skip _ZendTestClass class does not exist');
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
var_dump(class_exists('_ZendTestClassAlias'));
--EXPECT--
bool(true)
