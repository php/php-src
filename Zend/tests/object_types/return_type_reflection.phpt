--TEST--
Reflecting object return type
--FILE--
<?php

interface One {
    public function a() : object;
}

class Two implements One {
    public function a() : object {}
}

function a() : object {}

$returnTypeOne = (new ReflectionClass(One::class))->getMethod('a')->getReturnType();
var_dump($returnTypeOne->isBuiltin(), (string)$returnTypeOne);

$returnTypeTwo = (new ReflectionClass(Two::class))->getMethod('a')->getReturnType();
var_dump($returnTypeTwo->isBuiltin(), (string)$returnTypeTwo);

$returnTypea = (new ReflectionFunction('a'))->getReturnType();
var_dump($returnTypea->isBuiltin(), (string)$returnTypea);
--EXPECTF--
bool(true)
string(6) "object"
bool(true)
string(6) "object"
bool(true)
string(6) "object"
