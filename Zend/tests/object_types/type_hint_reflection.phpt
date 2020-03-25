--TEST--
Reflecting object type hint
--FILE--
<?php

interface One {
    public function a(object $obj);
}

class Two implements One {
    public function a(object $obj) {}
}

function a(object $obj) {}

$typeHintOne = (new ReflectionClass(One::class))->getMethod('a')->getParameters()[0]->getType();
var_dump($typeHintOne->isBuiltin(), $typeHintOne->getName());

$typeHintTwo = (new ReflectionClass(Two::class))->getMethod('a')->getParameters()[0]->getType();
var_dump($typeHintTwo->isBuiltin(), $typeHintTwo->getName());

$typeHinta = (new ReflectionFunction('a'))->getParameters()[0]->getType();
var_dump($typeHinta->isBuiltin(), $typeHinta->getName());
--EXPECT--
bool(true)
string(6) "object"
bool(true)
string(6) "object"
bool(true)
string(6) "object"
