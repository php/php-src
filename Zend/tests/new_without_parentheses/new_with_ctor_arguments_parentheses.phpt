--TEST--
Immediate access on new object with constructor arguments' parentheses
--FILE--
<?php

class A implements ArrayAccess
{
    const C = 'constant' . PHP_EOL;
    public $property = 'property' . PHP_EOL;
    public static $staticProperty = 'static property' . PHP_EOL;

    public function __invoke(): void
    {
        echo '__invoke' . PHP_EOL;
    }

    public function method(): void
    {
        echo 'method' . PHP_EOL;
    }

    public static function staticMethod(): void
    {
        echo 'static method' . PHP_EOL;
    }

    public function offsetExists(mixed $offset): bool
    {
        echo 'offsetExists' . PHP_EOL;

        return true;
    }

    public function offsetGet(mixed $offset): mixed
    {
        echo 'offsetGet' . PHP_EOL;

        return null;
    }

    public function offsetSet(mixed $offset, mixed $value): void {}

    public function offsetUnset(mixed $offset): void {}
}

$class = A::class;

echo new A()::C;
echo new A()::{'C'};
echo new $class()::C;
echo new $class()::{'C'};
echo new (trim(' A '))()::C;
echo new (trim(' A '))()::{'C'};

echo new A()->property;
echo new $class()->property;
echo new (trim(' A '))()->property;

echo new A()::$staticProperty;
echo new $class()::$staticProperty;
echo new (trim(' A '))()::$staticProperty;

new A()();
new $class()();
new (trim(' A '))()();

new A()->method();
new $class()->method();
new (trim(' A '))()->method();

new A()::staticMethod();
new $class()::staticMethod();
new (trim(' A '))()::staticMethod();

new A()['key'];
new $class()['key'];
new (trim(' A '))()['key'];

isset(new A()['key']);
isset(new $class()['key']);
isset(new (trim(' A '))()['key']);

?>
--EXPECT--
constant
constant
constant
constant
constant
constant
property
property
property
static property
static property
static property
__invoke
__invoke
__invoke
method
method
method
static method
static method
static method
offsetGet
offsetGet
offsetGet
offsetExists
offsetExists
offsetExists
