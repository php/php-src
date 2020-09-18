--TEST--
Attributes make use of closure scope.
--FILE--
<?php

class Test1
{
    private const FOO = 'bar';
}

class C1
{
    private const FOO = 'foo';

    public static function foo()
    {
        return #[A1(self::class, self::FOO)] function (#[A1(self::class, self::FOO)] $p) { };
    }
}

$ref = new \ReflectionFunction(C1::foo());
print_r($ref->getAttributes()[0]->getArguments());
print_r($ref->getParameters()[0]->getAttributes()[0]->getArguments());

echo "\n";

$ref = new \ReflectionFunction(C1::foo()->bindTo(null, Test1::class));
print_r($ref->getAttributes()[0]->getArguments());
print_r($ref->getParameters()[0]->getAttributes()[0]->getArguments());

?>
--EXPECT--
Array
(
    [0] => C1
    [1] => foo
)
Array
(
    [0] => C1
    [1] => foo
)

Array
(
    [0] => Test1
    [1] => bar
)
Array
(
    [0] => Test1
    [1] => bar
)
