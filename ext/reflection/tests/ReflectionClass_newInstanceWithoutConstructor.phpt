--TEST--
ReflectionClass::newInstanceWithoutConstructor()
--CREDITS--
Sebastian Bergmann <sebastian@php.net>
--FILE--
<?php
class Foo
{
    public function __construct()
    {
        print __METHOD__;
    }
}

$class = new ReflectionClass('Foo');
var_dump($class->newInstanceWithoutConstructor());

$class = new ReflectionClass('StdClass');
var_dump($class->newInstanceWithoutConstructor());

$class = new ReflectionClass('DateTime');
var_dump($class->newInstanceWithoutConstructor());

$class = new ReflectionClass('Generator');
var_dump($class->newInstanceWithoutConstructor());
--EXPECTF--
object(Foo)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(DateTime)#%d (0) {
}

Fatal error: Uncaught exception 'ReflectionException' with message 'Class Generator is an internal class marked as final that cannot be instantiated without invoking its constructor' in %sReflectionClass_newInstanceWithoutConstructor.php:%d
Stack trace:
#0 %sReflectionClass_newInstanceWithoutConstructor.php(%d): ReflectionClass->newInstanceWithoutConstructor()
#1 {main}
  thrown in %sReflectionClass_newInstanceWithoutConstructor.php on line %d
