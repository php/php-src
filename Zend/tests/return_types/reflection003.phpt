--TEST--
Reflection::getReturnType empty

--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 70000) {
    print 'skip';
}

--FILE--
<?php

$func = function() {
    return [];
};

$rf = new ReflectionFunction($func);
$rt = $rf->getReturnType();
var_dump($rt->getKind() == $rt::IS_UNDECLARED);
var_dump((string) $rt);
var_dump((string) $rt === $rt->getName());

--EXPECT--
bool(true)
string(0) ""
bool(true)
