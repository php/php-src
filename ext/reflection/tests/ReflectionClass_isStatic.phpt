--TEST--
Testing ReflectionClass::isStatic()
--FILE--
<?php

class C {}
static class C2 {}

var_dump(new ReflectionClass('C')->isStatic());
var_dump(new ReflectionClass('C2')->isStatic());
?>
--EXPECT--
bool(false)
bool(true)
