--TEST--
never return type: acceptable covariance cases
--FILE--
<?php

class A
{
    public function foo(): string
    {
        return "hello";
    }

    public function bar(): never
    {
        throw new UnexpectedValueException('parent');
    }

    public function &baz()
    {
    }

    public function someReturningStaticMethod() : static
    {
    }
}

class B extends A
{
    public function foo(): never
    {
        throw new UnexpectedValueException('bad');
    }

    public function bar(): never
    {
        throw new UnexpectedValueException('child');
    }

    public function &baz(): never
    {
        throw new UnexpectedValueException('child');
    }

    public function someReturningStaticMethod(): never
    {
        throw new UnexpectedValueException('child');
    }
}

try {
    (new B)->foo();
} catch (UnexpectedValueException $e) {
    // do nothing
}

try {
    (new B)->bar();
} catch (UnexpectedValueException $e) {
    // do nothing
}

try {
    (new B)->baz();
} catch (UnexpectedValueException $e) {
    // do nothing
}

try {
    (new B)->someReturningStaticMethod();
} catch (UnexpectedValueException $e) {
    // do nothing
}

echo "OK!", PHP_EOL;

?>
--EXPECT--
OK!
