--TEST--
Erasure: extends type arguments don't change runtime parent
--FILE--
<?php
class Base<T> {}
class Derived extends Base<int> {}
echo (new ReflectionClass('Derived'))->getParentClass()->getName(), "\n";
$d = new Derived;
var_dump($d instanceof Base);
?>
--EXPECT--
Base
bool(true)
