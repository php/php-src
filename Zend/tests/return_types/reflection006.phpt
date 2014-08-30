--TEST--
Reflection::getReturnType object

--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 70000) {
    print 'skip';
}

--FILE--
<?php

$func = function(): StdClass {
    return new StdClass;
};

$rf = new ReflectionFunction($func);
$rt = $rf->getReturnType();
var_dump($rt->getKind() == $rt::IS_OBJECT);
var_dump((string) $rt);
var_dump((string) $rt === $rt->getName());

--EXPECT--
bool(true)
string(8) "StdClass"
bool(true)
