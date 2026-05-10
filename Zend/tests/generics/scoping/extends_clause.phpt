--TEST--
Scoping: T is visible in extends clause
--FILE--
<?php
class Base<U> {}
class Derived<T : object> extends Base<T> {}
$rc = new ReflectionClass('Derived');
echo $rc->getParentClass()->getName(), "\n";
?>
--EXPECT--
Base
