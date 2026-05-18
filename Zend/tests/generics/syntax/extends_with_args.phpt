--TEST--
Generic syntax: extends with type arguments
--FILE--
<?php
class Base<T> {}
class Derived<U> extends Base<U> {}
echo (new ReflectionClass('Derived'))->getParentClass()->getName(), "\n";
?>
--EXPECT--
Base
