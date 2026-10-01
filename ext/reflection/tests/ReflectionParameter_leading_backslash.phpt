--TEST--
ReflectionParameter::__construct(): a leading "\" in the function name is ignored
--FILE--
<?php

function demo(string $arg) {}

$p = new ReflectionParameter("\\demo", 0);
var_dump($p->getName());

$p = new ReflectionParameter("demo", 0);
var_dump($p->getName());

?>
--EXPECT--
string(3) "arg"
string(3) "arg"
