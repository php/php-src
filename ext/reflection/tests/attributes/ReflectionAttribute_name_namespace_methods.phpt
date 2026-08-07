--TEST--
ReflectionAttribute name and namespace methods
--FILE--
<?php

namespace {

	#[Attribute]
	class AttrNotNamespace {}

}

namespace A\B {

	#[Attribute]
	class AttrNamespace {}

	class Foo {
	    #[\Attr]
	    public function fn1() {}

	    #[AttrNamespace]
	    public function fn2() {}
	}

	$rm = new \ReflectionMethod(Foo::class, "fn1");
	$attribute = $rm->getAttributes()[0];

	var_dump($attribute->inNamespace());
	var_dump($attribute->getName());
	var_dump($attribute->getNamespaceName());
	var_dump($attribute->getShortName());

	$rm = new \ReflectionMethod(Foo::class, "fn2");
	$attribute = $rm->getAttributes()[0];

	var_dump($attribute->inNamespace());
	var_dump($attribute->getName());
	var_dump($attribute->getNamespaceName());
	var_dump($attribute->getShortName());
}

?>
--EXPECT--
bool(false)
string(4) "Attr"
string(0) ""
string(4) "Attr"
bool(true)
string(17) "A\B\AttrNamespace"
string(3) "A\B"
string(13) "AttrNamespace"
