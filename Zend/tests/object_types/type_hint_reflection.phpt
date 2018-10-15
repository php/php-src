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
var_dump($typeHintOne->isBuiltin(), (string)$typeHintOne);

$typeHintTwo = (new ReflectionClass(Two::class))->getMethod('a')->getParameters()[0]->getType();
var_dump($typeHintTwo->isBuiltin(), (string)$typeHintTwo);

$typeHinta = (new ReflectionFunction('a'))->getParameters()[0]->getType();
var_dump($typeHinta->isBuiltin(), (string)$typeHinta);
--EXPECTF--
bool(true)
string(6) "object"
bool(true)
string(6) "object"
bool(true)
string(6) "object"
