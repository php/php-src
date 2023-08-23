--TEST--
Using null as offset must make the value accessible
--EXTENSIONS--
spl
--FILE--
<?php
class MyClass {}
$c = new MyClass();

$ao = new ArrayObject($c);

$ao->offsetSet(null, "null key");
var_dump($ao->offsetExists(null));
var_dump($ao->offsetGet(null));
var_dump($c);
var_dump($c->{""});
?>
--EXPECTF--
Deprecated: Creation of dynamic property MyClass::$ is deprecated in %s on line %d
bool(true)
string(8) "null key"
object(MyClass)#1 (1) {
  [""]=>
  string(8) "null key"
}
string(8) "null key"
