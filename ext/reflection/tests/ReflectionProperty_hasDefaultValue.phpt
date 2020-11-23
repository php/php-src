--TEST--
reflection: ReflectionProperty::hasDefaultValue
--FILE--
<?php

class TestClass
{
    public $foo;
    public $bar = 'baz';

    public static $static1;
    public static $static2 = 1234;

    public int $val1;
    public int $val2 = 1234;

    public ?int $nullable;
    public ?int $nullable2 = null;
}

$property = new ReflectionProperty(TestClass::class, 'foo');
var_dump($property->hasDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'bar');
var_dump($property->hasDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'static1');
var_dump($property->hasDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'static2');
var_dump($property->hasDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'val1');
var_dump($property->hasDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'val2');
var_dump($property->hasDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'nullable');
var_dump($property->hasDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'nullable2');
var_dump($property->hasDefaultValue());

$test = new TestClass;
$test->dynamic = null;
$property = new ReflectionProperty($test, 'dynamic');
var_dump($property->hasDefaultValue());

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
