--TEST--
ReflectionFunction::get{Short,Namespace}Name() and inNamespace() return the correct data for closures defined in namespaces.
--FILE--
<?php
namespace Foo;

class Bar {
	public function baz() {
		return function () {

		};
	}
}

$c = (new Bar())->baz();
$r = new \ReflectionFunction($c);
// Closures are not inside of a namespace, thus the short name is the full name.
var_dump($r->getShortName());
// The namespace is empty.
var_dump($r->getNamespaceName());
// The function is not inside of a namespace.
var_dump($r->inNamespace());
// And the namespace name + the short name together must be the full name.
var_dump($r->getNamespaceName() . ($r->inNamespace() ? '\\' : '') . $r->getShortName() === $r->getName());
?>
--EXPECT--
string(26) "{closure:Foo\Bar::baz():6}"
string(0) ""
bool(false)
bool(true)
