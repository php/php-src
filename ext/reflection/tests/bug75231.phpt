--TEST--
Bug #75231: ReflectionProperty#getValue() incorrectly works with inherited classes
--FILE--
<?php
class A
{
    public $prop;
    public function __construct()
    {
        $this->prop = 'prop';
    }
    public function method()
    {
        return 'method';
    }
}
class B extends A
{
}
print_r((new ReflectionMethod(B::class, 'method'))->invoke(new A()).PHP_EOL);
print_r((new ReflectionProperty(B::class, 'prop'))->getValue(new A()).PHP_EOL);
?>
--EXPECT--
method
prop
