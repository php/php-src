--TEST--
Bug #72174: ReflectionProperty#getValue() causes __isset call
--FILE--
<?php

class Foo
{
    private $bar;

    public function __construct()
    {
        unset($this->bar);
    }

    public function __isset($name)
    {
        var_dump(__METHOD__);
        return true;
    }

    public function __get($name)
    {
        var_dump(__METHOD__);
        return $name;
    }
}

$instance = new Foo();
$reflectionBar = (new ReflectionProperty(Foo::class, 'bar'));
$reflectionBar->setAccessible(true);
var_dump($reflectionBar->getValue($instance));

?>
--EXPECT--
string(10) "Foo::__get"
string(3) "bar"
