--TEST--
Verify that variadic arguments create proper stub
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflection = new ReflectionMethod("_ZendTestClass", "variadicTest");
$arguments = $reflection->getParameters();

echo (string) $arguments[0], "\n";
var_dump($arguments[0]->isVariadic());

$type = $arguments[0]->getType();

var_dump($type instanceof ReflectionUnionType);

echo "\n";

$types = $type->getTypes();

var_dump($types[0]->getName());
var_dump($types[0] instanceof ReflectionNamedType);
var_dump($types[0]->allowsNull());

echo "\n";

var_dump($types[1]->getName());
var_dump($types[1] instanceof ReflectionNamedType);
var_dump($types[1]->allowsNull());

?>
--EXPECTF--
Parameter #0 [ <optional> Iterator|string ...$elements ]
bool(true)
bool(true)

string(8) "Iterator"
bool(true)
bool(false)

string(6) "string"
bool(true)
bool(false)
