--TEST--
ReflectionFunction::invoke() preserves the called scope of static closures
--FILE--
<?php
class BaseClass
{
    public static function callback(): Closure
    {
        return static fn () => self::class . ', ' . static::class;
    }
}

class ChildClass extends BaseClass
{
}

$reflection = new ReflectionFunction(ChildClass::callback());
echo $reflection->invoke(), "\n";
echo $reflection->invokeArgs([]), "\n";
?>
--EXPECT--
BaseClass, ChildClass
BaseClass, ChildClass
