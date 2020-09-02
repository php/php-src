--TEST--
Test ReflectionZendExtension class errors
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--SKIPIF--
<?php if(!extension_loaded('Zend OPcache')) die('skip Zend OPcache extension not loaded'); ?>
--FILE--
<?php
try {
    new ReflectionZendExtension('zend_opcache');
} catch (ReflectionException $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Zend Extension "zend_opcache" does not exist
