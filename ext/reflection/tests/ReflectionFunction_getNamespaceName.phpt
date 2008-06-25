--TEST--
ReflectionFunction::getNamespaceName()
--FILE--
<?php
namespace A::B;
function foo() {}

$function = new ReflectionFunction('sort');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());

$function = new ReflectionFunction('A::B::foo');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());
?>
--EXPECT--
bool(false)
unicode(4) "sort"
unicode(0) ""
unicode(4) "sort"
bool(true)
unicode(9) "A::B::foo"
unicode(4) "A::B"
unicode(3) "foo"

