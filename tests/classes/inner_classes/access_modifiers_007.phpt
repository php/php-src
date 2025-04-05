--TEST--
abstract inner classes
--FILE--
<?php

class Outer {
    public abstract class Inner {}
}

class Extended extends Outer\Inner {}

$extended = new Extended();

var_dump($extended);
$reflection = new ReflectionClass('Outer\Inner');
var_dump($reflection->isAbstract());
new Outer\Inner();
?>
--EXPECTF--
object(Extended)#1 (0) {
}
bool(true)

Fatal error: Uncaught Error: Cannot instantiate abstract class Outer\Inner in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
