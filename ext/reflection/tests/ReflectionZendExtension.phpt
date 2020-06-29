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
?>
--EXPECT--
string(17) "Zend Technologies"
string(13) "Copyright (c)"
string(12) "Zend OPcache"
string(20) "http://www.zend.com/"
bool(true)
