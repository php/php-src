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

echo "OK!", PHP_EOL;

?>
--EXPECT--
OK!
