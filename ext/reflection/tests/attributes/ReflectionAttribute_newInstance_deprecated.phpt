--TEST--
ReflectionAttribute::newInstance(): #[\Deprecated]
--FILE--
<?php

#[\Deprecated]
function test1() {
}

#[\Deprecated()]
function test2() {
}

#[\Deprecated("use test() instead")]
function test3() {
}

#[\Deprecated(since: "2.0")]
function test4() {
}

$reflection = new ReflectionFunction('test1');
var_dump($reflection->getAttributes()[0]->newInstance());

$reflection = new ReflectionFunction('test2');
var_dump($reflection->getAttributes()[0]->newInstance());

$reflection = new ReflectionFunction('test3');
var_dump($reflection->getAttributes()[0]->newInstance());

$reflection = new ReflectionFunction('test4');
var_dump($reflection->getAttributes()[0]->newInstance());

?>
--EXPECTF--
object(Deprecated)#%d (2) {
  ["message"]=>
  NULL
  ["since"]=>
  NULL
}
object(Deprecated)#%d (2) {
  ["message"]=>
  NULL
  ["since"]=>
  NULL
}
object(Deprecated)#%d (2) {
  ["message"]=>
  string(18) "use test() instead"
  ["since"]=>
  NULL
}
object(Deprecated)#%d (2) {
  ["message"]=>
  NULL
  ["since"]=>
  string(3) "2.0"
}
