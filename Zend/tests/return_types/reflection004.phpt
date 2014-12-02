--TEST--
Reflection::getReturnType array

--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 70000) {
    print 'skip';
}

--FILE--
<?php

$func = function(): array {
    return [];
};

$rf = new ReflectionFunction($func);
$rt = $rf->getReturnType();
var_dump($rt->getKind() == $rt::IS_ARRAY);
var_dump((string) $rt);
var_dump((string) $rt === $rt->getName());

--EXPECT--
bool(true)
string(5) "array"
bool(true)
