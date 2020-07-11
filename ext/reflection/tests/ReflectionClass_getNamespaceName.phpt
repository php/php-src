--TEST--
ReflectionClass::getNamespaceName()
--FILE--
<?php
namespace A\B;
class Foo {
}

$function = new \ReflectionClass('stdClass');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());

$function = new \ReflectionClass('A\\B\\Foo');
var_dump($function->inNamespace());
var_dump($function->getName());
var_dump($function->getNamespaceName());
var_dump($function->getShortName());
?>
--EXPECT--
bool(false)
string(8) "stdClass"
string(0) ""
string(8) "stdClass"
bool(true)
string(7) "A\B\Foo"
string(3) "A\B"
string(3) "Foo"
