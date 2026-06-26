--TEST--
Scoping: T is visible in implements clause
--FILE--
<?php
interface IConsumer<X> {}
class Sink<T : object> implements IConsumer<T> {}
$rc = new ReflectionClass('Sink');
echo $rc->getInterfaceNames()[0], "\n";
?>
--EXPECT--
IConsumer
