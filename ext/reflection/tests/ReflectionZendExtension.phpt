--TEST--
Test ReflectionZendExtension class
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--SKIPIF--
<?php if(!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not loaded'); ?>
--FILE--
<?php
$reflection = new ReflectionZendExtension('Zend OPcache');
var_dump($reflection->getAuthor());
var_dump($reflection->getCopyright());
var_dump($reflection->getName());
var_dump($reflection->getURL());
var_dump($reflection->getVersion() === PHP_VERSION);
var_dump(gettype($reflection->export('Zend OPcache', true)) === 'string');
?>
--EXPECTF--
string(17) "Zend Technologies"
string(23) "Copyright (c) 1999-20%d"
string(12) "Zend OPcache"
string(20) "http://www.zend.com/"
bool(true)
bool(true)
