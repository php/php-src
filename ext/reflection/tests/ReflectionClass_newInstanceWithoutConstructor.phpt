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
try {
    var_dump($class->newInstanceWithoutConstructor());
} catch (ReflectionException $e) {
    echo $e->getMessage(), "\n";
}

final class Bar extends ArrayObject {
}

$class = new ReflectionClass('Bar');
var_dump($class->newInstanceWithoutConstructor());

?>
--EXPECTF--
object(Foo)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(DateTime)#%d (0) {
}
Class Generator is an internal class marked as final that cannot be instantiated without invoking its constructor
object(Bar)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
