--TEST--
Bug #50810 (property_exists does not work for private)
--FILE--
<?php

class ExampleSuperClass
{
    private $foo;
    static protected $bar;
 
    private function foo()
    {
    }
 
    public function propertyFooExists()
    {
        return property_exists($this, 'foo');
    }
 
}
 
class ExampleSubClass extends ExampleSuperClass
{
    public function methodExists()
    {
        return method_exists($this, 'foo');
    }
 
    public function propertyBarExists()
    {
        return property_exists($this, 'bar');
    }
}
 
$example = new ExampleSubClass();
var_dump($example->methodExists());
var_dump(method_exists($example, 'propertyFooExists'));
var_dump($example->propertyFooExists());
var_dump($example->propertyBarExists());

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
