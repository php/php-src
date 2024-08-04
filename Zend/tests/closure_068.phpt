--TEST--
ReflectionFunction::getShortName() returns the short name for first class callables defined in namespaces.
--FILE--
<?php
namespace Foo;

function foo() {
}
$r = new \ReflectionFunction(foo(...));
var_dump($r->getShortName());
?>
--EXPECT--
string(3) "foo"
