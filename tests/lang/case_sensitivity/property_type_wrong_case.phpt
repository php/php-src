--TEST--
Property type declarations with wrong-cased class names fails with wrong case
--FILE--
<?php
class MyClass {}

class Container {
    public MYCLASS $value;
}

$c = new Container();
$c->value = new MyClass();
echo get_class($c->value) . "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot assign MyClass to property Container::$value of type MYCLASS in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
