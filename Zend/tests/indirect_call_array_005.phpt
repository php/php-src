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

$callback = ['TestClass', 'method'];
echo $callback();
?>
--EXPECTF--
Deprecated: Non-static method TestClass::method() should not be called statically in %s on line %d

Fatal error: Uncaught Error: Using $this when not in object context in %s:%d
Stack trace:
#0 %s(%d): TestClass::method()
#1 {main}
  thrown in %s on line %d
