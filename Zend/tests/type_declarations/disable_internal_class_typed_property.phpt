--TEST--
Disable class with typed properties
--EXTENSIONS--
reflection
--INI--
disable_classes=Exception
--XFAIL--
Use After Free due to types being freed by zend_disable_class()
--FILE--
<?php

//$o = new ReflectionMethod(Exception::class, "__toString");
//var_dump($o);

$o = new Exception();
//var_dump($o);
?>
--EXPECTF--
Warning: ReflectionMethod() has been disabled for security reasons in %s
