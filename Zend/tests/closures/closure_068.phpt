--TEST--
ReflectionFunction::get{Short,Namespace}Name() and inNamespace() return the correct data for first class callables defined in namespaces.
--FILE--
<?php
namespace Foo;

function foo() {
}
$r = new \ReflectionFunction(foo(...));
$r2 = new \ReflectionFunction('Foo\\foo');
var_dump($r->getShortName());
var_dump($r->getNamespaceName());
var_dump($r->inNamespace());
var_dump($r->getNamespaceName() . ($r->inNamespace() ? '\\' : '') . $r->getShortName() === $r->getName());

var_dump($r->getShortName() === $r2->getShortName());
var_dump($r->getNamespaceName() === $r2->getNamespaceName());
var_dump($r->inNamespace() === $r2->inNamespace());
?>
--EXPECT--
string(3) "foo"
string(3) "Foo"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
