--TEST--
Dynamic static call to instance method should throw.
--CREDITS--
Aaron Piotrowski <aaron@icicle.io>
--FILE--
<?php
class TestClass
{
    private $test;

    public function method()
    {
        $this->test = 'test';
        return "Hello, world!\n";
    }
}

$callback = 'TestClass::method';
echo $callback();
?>
--EXPECTF--
Fatal error: Uncaught Error: Non-static method TestClass::method() cannot be called statically in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
