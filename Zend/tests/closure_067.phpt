--TEST--
ReflectionFunction::getShortName() returns the full name for closures defined in namespaces.
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
var_dump($r->getShortName());
?>
--EXPECT--
string(26) "{closure:Foo\Bar::baz():6}"
