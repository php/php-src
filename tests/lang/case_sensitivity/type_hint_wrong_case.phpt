--TEST--
Type declarations with wrong-cased class names fails with wrong case
--FILE--
<?php
class MyClass {}

function acceptMyClass(MYCLASS $x): MYCLASS { return $x; }

$obj = new MyClass();
$result = acceptMyClass($obj);
echo get_class($result) . "\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: acceptMyClass(): Argument #1 ($x) must be of type MYCLASS, MyClass given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(7): acceptMyClass(Object(MyClass))
#1 {main}
  thrown in %s on line %d
