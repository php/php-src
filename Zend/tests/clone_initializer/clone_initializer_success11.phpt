--TEST--
Test that the property initializer list can contain expressions
--FILE--
<?php

class Foo
{
    private $property1;

    public function withProperty1()
    {
        $property = "property1";

        return clone $this with [$property => "value"];
    }

    public function withProperty2()
    {
        $property = "string";

        return clone $this with [$this->getProperty1Name() => "value"];
    }

    public function withProperty3()
    {
        $property = "property2";

        return clone $this with [$property => "value"];
    }

    public function withProperty4()
    {
        return clone $this with [substr($this->getProperty1Name(), 0, 8) . "1" => "value"];
    }

    private function getProperty1Name() {
        return "property1";
    }
}

$obj = new Foo();
$obj1 = $obj->withProperty1();
$obj2 = $obj->withProperty1();
$obj3 = $obj->withProperty2();
$obj4 = $obj->withProperty2();
$obj5 = $obj->withProperty3();
$obj6 = $obj->withProperty3();
$obj7 = $obj->withProperty4();
$obj8 = $obj->withProperty4();

var_dump($obj1);
var_dump($obj2);
var_dump($obj3);
var_dump($obj4);
var_dump($obj5);
var_dump($obj6);
var_dump($obj7);
var_dump($obj8);

?>
--EXPECTF--
Deprecated: Creation of dynamic property Foo::$property2 is deprecated in %s on line %d

Deprecated: Creation of dynamic property Foo::$property2 is deprecated in %s on line %d
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  string(5) "value"
}
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  string(5) "value"
}
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  string(5) "value"
}
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  string(5) "value"
}
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  NULL
  ["property2"]=>
  string(5) "value"
}
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  NULL
  ["property2"]=>
  string(5) "value"
}
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  string(5) "value"
}
object(Foo)#%d (%d) {
  ["property1":"Foo":private]=>
  string(5) "value"
}
