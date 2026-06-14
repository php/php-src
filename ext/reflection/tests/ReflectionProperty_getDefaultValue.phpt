--TEST--
reflection: ReflectionProperty::getDefaultValue
--FILE--
<?php

define('FOO', 42);

#[AllowDynamicProperties]
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

    public $constantAst = 2 * 2;
    public $constantRuntimeAst = FOO;
}

$property = new ReflectionProperty(TestClass::class, 'foo');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'bar');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'static1');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'static2');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'val1');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'val2');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'nullable');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'nullable2');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'constantAst');
var_dump($property->getDefaultValue());

$property = new ReflectionProperty(TestClass::class, 'constantRuntimeAst');
var_dump($property->getDefaultValue());

$test = new TestClass;
$test->dynamic = null;
$property = new ReflectionProperty($test, 'dynamic');
var_dump($property->getDefaultValue());

?>
--EXPECTF--
NULL
string(3) "baz"
NULL
int(1234)

Deprecated: ReflectionProperty::getDefaultValue() for a property without a default value is deprecated, use ReflectionProperty::hasDefaultValue() to check if the default value exists in %s on line %d
NULL
int(1234)

Deprecated: ReflectionProperty::getDefaultValue() for a property without a default value is deprecated, use ReflectionProperty::hasDefaultValue() to check if the default value exists in %s on line %d
NULL
NULL
int(4)
int(42)

Deprecated: ReflectionProperty::getDefaultValue() for a property without a default value is deprecated, use ReflectionProperty::hasDefaultValue() to check if the default value exists in %s on line %d
NULL
