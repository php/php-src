--TEST--
ReflectionFunction::isVariadic()
--FILE--
<?php

function test1($args) {}
function test2(...$args) {}
function test3($arg, ...$args) {}

var_dump((new ReflectionFunction('test1'))->isVariadic());
var_dump((new ReflectionFunction('test2'))->isVariadic());
var_dump((new ReflectionFunction('test3'))->isVariadic());

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
