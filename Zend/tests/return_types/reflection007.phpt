--TEST--
Reflection::getReturnType object

--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 70000) {
    print 'skip';
}

--FILE--
<?php

interface A {
    function foo(): A;
}

$rc = new ReflectionClass("A");
$rt = $rc->getMethod("foo")->getReturnType();
var_dump($rt->getKind() == $rt::IS_OBJECT);
var_dump((string) $rt);
var_dump((string) $rt === $rt->getName());

--EXPECT--
bool(true)
string(1) "A"
bool(true)
