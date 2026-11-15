--TEST--
Test ReflectionProperty::isReadable() indirect
--FILE--
<?php

class ParentClass {
    public static int $prop;
}

class ChildClass extends ParentClass {}

var_dump((new ReflectionProperty('ParentClass', 'prop'))->isReadable(null));
var_dump((new ReflectionProperty('ChildClass', 'prop'))->isReadable(null));

ChildClass::$prop = 1;

var_dump((new ReflectionProperty('ParentClass', 'prop'))->isReadable(null));
var_dump((new ReflectionProperty('ChildClass', 'prop'))->isReadable(null));

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
