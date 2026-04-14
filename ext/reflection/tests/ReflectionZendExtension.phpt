--TEST--
Test ReflectionZendExtension class
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--EXTENSIONS--
opcache
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
string(16) "Zend by Perforce"
string(12) "Copyright ©"
string(12) "Zend OPcache"
string(21) "https://www.zend.com/"
bool(true)
