--TEST--
PFA reflection: ReflectionFunction::isClosure() is true for partials
--FILE--
<?php

var_dump((new ReflectionFunction('sprintf'))->isClosure());

var_dump((new ReflectionFunction(function () {}))->isClosure());

var_dump((new ReflectionFunction(sprintf(?)))->isClosure());

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
