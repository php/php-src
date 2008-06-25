--TEST--
ReflectionClass::getNamespaceName()
--FILE--
<?php
namespace A::B;
class Foo {
}

$function = new ReflectionClass('stdClass');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());

$function = new ReflectionClass('A::B::Foo');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());
?>
--EXPECT--
bool(false)
unicode(8) "stdClass"
unicode(0) ""
unicode(8) "stdClass"
bool(true)
unicode(9) "A::B::Foo"
unicode(4) "A::B"
unicode(3) "Foo"

