--TEST--
Using null as key must make the value accessible
--EXTENSIONS--
spl
--FILE--
<?php
class MyClass {}
$c = new MyClass();

$ao = new ArrayObject($c);

$ao[null] = "null key";
var_dump($c);
var_dump($c->{""});
var_dump($ao[null]);
?>
--EXPECTF--
Deprecated: Creation of dynamic property MyClass::$ is deprecated in %s on line %d
object(MyClass)#1 (1) {
  [""]=>
  string(8) "null key"
}
string(8) "null key"
string(8) "null key"
