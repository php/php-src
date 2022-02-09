--TEST--
Verify that parameter attributes for native functions correctly support arguments.
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflection = new ReflectionFunction("zend_test_parameter_with_attribute");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

$reflection = new ReflectionMethod("ZendTestClassWithMethodWithParameterAttribute", "no_override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

$reflection = new ReflectionMethod("ZendTestClassWithMethodWithParameterAttribute", "override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

$reflection = new ReflectionMethod("ZendTestChildClassWithMethodWithParameterAttribute", "no_override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

$reflection = new ReflectionMethod("ZendTestChildClassWithMethodWithParameterAttribute", "override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

class ChildClassWithNoAttribute extends ZendTestClassWithMethodWithParameterAttribute {
	public function override(string $parameter): int
	{
		return 5;
	}
}

$reflection = new ReflectionMethod("ChildClassWithNoAttribute", "no_override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

$reflection = new ReflectionMethod("ChildClassWithNoAttribute", "override");
var_dump(count($reflection->getParameters()[0]->getAttributes()));

class ChildClassWithSameAttribute extends ZendTestClassWithMethodWithParameterAttribute {
	public function override(#[ZendTestParameterAttribute("value5")] string $parameter): int
	{
		return 6;
	}
}

$reflection = new ReflectionMethod("ChildClassWithSameAttribute", "no_override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

$reflection = new ReflectionMethod("ChildClassWithSameAttribute", "override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

#[\Attribute(\Attribute::TARGET_PARAMETER)]
class SomeAttribute {
  public function __construct(public string $someParam) { }
}

class ChildClassWithDifferentAttribute extends ZendTestClassWithMethodWithParameterAttribute {
	public function override(#[SomeAttribute("value6")] string $parameter): int
	{
		return 7;
	}
}

$reflection = new ReflectionMethod("ChildClassWithDifferentAttribute", "no_override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

$reflection = new ReflectionMethod("ChildClassWithDifferentAttribute", "override");
$attribute = $reflection->getParameters()[0]->getAttributes()[0];
var_dump($attribute->getArguments());
var_dump($attribute->newInstance());

?>
--EXPECTF--
array(1) {
  [0]=>
  string(6) "value1"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value1"
}
array(1) {
  [0]=>
  string(6) "value2"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value2"
}
array(1) {
  [0]=>
  string(6) "value3"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value3"
}
array(1) {
  [0]=>
  string(6) "value2"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value2"
}
array(1) {
  [0]=>
  string(6) "value4"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value4"
}
array(1) {
  [0]=>
  string(6) "value2"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value2"
}
int(0)
array(1) {
  [0]=>
  string(6) "value2"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value2"
}
array(1) {
  [0]=>
  string(6) "value5"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value5"
}
array(1) {
  [0]=>
  string(6) "value2"
}
object(ZendTestParameterAttribute)#%d (1) {
  ["parameter"]=>
  string(6) "value2"
}
array(1) {
  [0]=>
  string(6) "value6"
}
object(SomeAttribute)#%d (1) {
  ["someParam"]=>
  string(6) "value6"
}
