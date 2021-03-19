--TEST--
noreturn return type: acceptable covariance cases
--FILE--
<?php

class A
{
    public function foo(): string
    {
        return "hello";
    }

    public function bar(): noreturn
    {
        throw new UnexpectedValueException('parent');
    }

    public function &baz()
    {
    }
}

class B extends A
{
    public function foo(): noreturn
    {
        throw new UnexpectedValueException('bad');
    }

    public function bar(): noreturn
    {
        throw new UnexpectedValueException('child');
    }

    public function &baz(): noreturn
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

echo "OK!", PHP_EOL;

?>
--EXPECT--
OK!
