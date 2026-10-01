--TEST--
ReflectionClass::getProperty() with a qualified anonymous class name
--FILE--
<?php

$obj = new class {
    public $p = 42;
};

$reflector = new ReflectionClass($obj);
$name = $reflector->getName();
var_dump(str_contains($name, "\0"));

$p = $reflector->getProperty($name . '::p');
var_dump($p->getName(), $p->getValue($obj));

?>
--EXPECT--
bool(true)
string(1) "p"
int(42)
