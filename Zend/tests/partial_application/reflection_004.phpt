--TEST--
PFA reflection: ReflectionFunction::isAnonymous() is true for partials
--FILE--
<?php

var_dump((new ReflectionFunction('sprintf'))->isAnonymous());

var_dump((new ReflectionFunction(function () {}))->isAnonymous());

var_dump((new ReflectionFunction(sprintf(?)))->isAnonymous());

?>
--EXPECT--
bool(false)
bool(true)
bool(true)
