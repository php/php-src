--TEST--
class_parents(), class_implements(), class_uses() with wrong-case class name fails with wrong case
--FILE--
<?php
class Base {}
class Child extends Base {}
interface MyInterface {}
class Impl implements MyInterface {}
trait MyTrait {}
class WithTrait { use MyTrait; }

$parents = class_parents("CHILD");
echo implode(", ", array_keys($parents)) . "\n";

$interfaces = class_implements("IMPL");
echo implode(", ", array_keys($interfaces)) . "\n";

$traits = class_uses("WITHTRAIT");
echo implode(", ", array_keys($traits)) . "\n";
?>
--EXPECTF--
Warning: class_parents(): Class CHILD does not exist and could not be loaded in %s on line %d

Fatal error: Uncaught TypeError: array_keys(): Argument #1 ($array) must be of type array, false given in %s:%d
Stack trace:
#0 %s(10): array_keys(false)
#1 {main}
  thrown in %s on line %d
