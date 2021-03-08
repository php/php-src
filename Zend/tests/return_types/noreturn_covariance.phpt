--TEST--
noreturn return type: acceptable covariance cases
--FILE--
<?php

class A
{
    public function foo() : string
    {
        return "hello";
    }

    public function bar() : noreturn
    {
        throw new \Exception('parent');
    }
}

class B extends A
{
    public function foo() : noreturn
    {
        throw new \Exception('bad');
    }

    public function bar() : noreturn
    {
        throw new \Exception('child');
    }
}

try {
    (new B)->foo();
} catch (Exception $e) {
    // do nothing
}

try {
    (new B)->bar();
} catch (Exception $e) {
    // do nothing
}

echo "OK!", PHP_EOL;

?>
--EXPECT--
OK!
