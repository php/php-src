--TEST--
abstract inner classes
--FILE--
<?php

class Outer {
    abstract class Inner {}
}

$extended = new class extends Outer:>Inner{};

var_dump($extended);
$reflection = new ReflectionClass('Outer:>Inner');
var_dump($reflection->isAbstract());
new Outer:>Inner();
?>
--EXPECTF--
last one fails
