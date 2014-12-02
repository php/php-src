--TEST--
Reflection::hasReturnType true

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
var_dump($rf->hasReturnType());

--EXPECT--
bool(true)
