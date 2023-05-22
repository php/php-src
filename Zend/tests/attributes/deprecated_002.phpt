--TEST--
#[Deprecated] attribute
--FILE--
<?php

#[Deprecated()]
function test() {
}

#[Deprecated("use test() instead")]
function test2() {
}

$reflection = new ReflectionFunction('test');
var_dump($reflection->getAttributes()[0]->newInstance());

$reflection = new ReflectionFunction('test2');
var_dump($reflection->getAttributes()[0]->newInstance());

--EXPECTF--
object(Deprecated)#3 (1) {
  ["message"]=>
  NULL
}
object(Deprecated)#2 (1) {
  ["message"]=>
  string(18) "use test() instead"
}
