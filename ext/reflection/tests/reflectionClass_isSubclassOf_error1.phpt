--TEST--
ReflectionClass::isSubclassOf() - non-existent class error
--FILE--
<?php
class A {}
$rc = new ReflectionClass('A');

var_dump($rc->isSubclassOf('X'));

?>
--EXPECTF--
Fatal error: Uncaught exception 'ReflectionException' with message 'Class X does not exist' in %s:5
Stack trace:
#0 %s(5): ReflectionClass->isSubclassOf('X')
#1 {main}
  thrown in %s on line 5
