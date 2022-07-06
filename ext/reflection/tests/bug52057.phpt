--TEST--
Bug #52057 (ReflectionClass fails on Closure class)
--FILE--
<?php

$closure = function($a) { echo $a; };

$reflection = new ReflectionClass('closure');
var_dump($reflection->hasMethod('__invoke')); // true

$reflection = new ReflectionClass($closure);
var_dump($reflection->hasMethod('__invoke')); // true

$reflection = new ReflectionObject($closure);
var_dump($reflection->hasMethod('__invoke')); // true

$reflection = new ReflectionClass('closure');
var_dump($h = $reflection->getMethod('__invoke')); // true
var_dump($h->class.'::'.$h->getName());

$reflection = new ReflectionClass($closure);
var_dump($h = $reflection->getMethod('__invoke')); // true
var_dump($h->class.'::'.$h->getName());

$reflection = new ReflectionObject($closure);
var_dump($h = $reflection->getMethod('__invoke')); // true
var_dump($h->class.'::'.$h->getName());

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
object(ReflectionMethod)#%d (2) {
  ["name"]=>
  string(8) "__invoke"
  ["class"]=>
  string(7) "Closure"
}
string(17) "Closure::__invoke"
object(ReflectionMethod)#%d (2) {
  ["name"]=>
  string(8) "__invoke"
  ["class"]=>
  string(7) "Closure"
}
string(17) "Closure::__invoke"
object(ReflectionMethod)#%d (2) {
  ["name"]=>
  string(8) "__invoke"
  ["class"]=>
  string(7) "Closure"
}
string(17) "Closure::__invoke"
