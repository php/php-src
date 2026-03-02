--TEST--
Verify that parameter attributes for native functions do not leak.
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(zend_test_parameter_with_attribute("foo"));

$o = new ZendTestClassWithMethodWithParameterAttribute();
var_dump($o->no_override("foo"));
var_dump($o->override("foo"));

$o = new ZendTestChildClassWithMethodWithParameterAttribute();
var_dump($o->no_override("foo"));
var_dump($o->override("foo"));

class ChildClassWithNoAttribute extends ZendTestClassWithMethodWithParameterAttribute {
	public function override(string $parameter): int
	{
		return 5;
	}
}

$o = new ChildClassWithNoAttribute();
var_dump($o->no_override("foo"));
var_dump($o->override("foo"));

class ChildClassWithSameAttribute extends ZendTestClassWithMethodWithParameterAttribute {
	public function override(#[ZendTestParameterAttribute] string $parameter): int
	{
		return 6;
	}
}

$o = new ChildClassWithSameAttribute();
var_dump($o->no_override("foo"));
var_dump($o->override("foo"));

#[\Attribute(\Attribute::TARGET_PARAMETER)]
class SomeAttribute {

}

class ChildClassWithDifferentAttribute extends ZendTestClassWithMethodWithParameterAttribute {
	public function override(#[SomeAttribute] string $parameter): int
	{
		return 7;
	}
}

$o = new ChildClassWithDifferentAttribute();
var_dump($o->no_override("foo"));
var_dump($o->override("foo"));

?>
--EXPECT--
int(1)
int(2)
int(3)
int(2)
int(4)
int(2)
int(5)
int(2)
int(6)
int(2)
int(7)
