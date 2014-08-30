--TEST--
Reflection::getReturnType callable

--SKIPIF--
<?php
if (!extension_loaded('reflection') || !defined('PHP_VERSION_ID') || PHP_VERSION_ID < 70000) {
    print 'skip';
}

--FILE--
<?php

$func = function(): callable {
    return 'strlen';
};

$rf = new ReflectionFunction($func);
$rt = $rf->getReturnType();
var_dump($rt->getKind() == $rt::IS_CALLABLE);
var_dump((string) $rt);
var_dump((string) $rt === $rt->getName());

--EXPECT--
bool(true)
string(8) "callable"
bool(true)
