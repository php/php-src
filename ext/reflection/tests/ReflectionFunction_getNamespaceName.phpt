--TEST--
ReflectionFunction::getNamespaceName()
--FILE--
<?php
namespace A\B;
function foo() {}

$function = new \ReflectionFunction('sort');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());

$function = new \ReflectionFunction('A\\B\\foo');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());
?>
--EXPECT--
bool(false)
string(4) "sort"
string(0) ""
string(4) "sort"
bool(true)
string(7) "A\B\foo"
string(3) "A\B"
string(3) "foo"

