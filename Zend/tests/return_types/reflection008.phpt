--TEST--
Reflection::getReturnType object

--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 70000) {
    print 'skip';
}

--FILE--
<?php

class A {
    function foo(): array {
        return [];
    }
}

$rc = new ReflectionClass("A");
$rt = $rc->getMethod("foo")->getReturnType();
var_dump($rt->getKind() == $rt::IS_ARRAY);
var_dump((string) $rt);
var_dump((string) $rt === $rt->getName());

--EXPECT--
bool(true)
string(5) "array"
bool(true)
