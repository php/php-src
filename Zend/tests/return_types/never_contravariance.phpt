--TEST--
never return type: acceptable contravariance cases
--FILE--
<?php

class A
{
    public function foo(never $var)
    {
        return "hello";
    }

    public function bar(never $var)
    {
        throw new UnexpectedValueException('parent');
    }

    public function &baz(never $var)
    {
    }

    public function someReturningStaticMethod(never $var)
    {
    }
}

class B extends A
{
    public function foo(int $var)
    {
        throw new UnexpectedValueException('bad');
    }

    public function bar(int|float $var)
    {
        throw new UnexpectedValueException('child');
    }

    public function &baz(mixed $var)
    {
        throw new UnexpectedValueException('child');
    }

    public function someReturningStaticMethod(string $var)
    {
        throw new UnexpectedValueException('child');
    }
}

try {
    (new B)->foo(1);
} catch (UnexpectedValueException $e) {
    // do nothing
}

try {
    (new B)->bar(1.0);
} catch (UnexpectedValueException $e) {
    // do nothing
}

try {
    (new B)->baz([]);
} catch (UnexpectedValueException $e) {
    // do nothing
}

try {
    (new B)->someReturningStaticMethod("some value");
} catch (UnexpectedValueException $e) {
    // do nothing
}

echo "OK!", PHP_EOL;

?>
--EXPECT--
OK!
