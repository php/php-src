--TEST--
Bug #70685: Segfault for getClosure() internal method rebind with invalid $this
--FILE--
<?php

class cls {}

$c = (new ReflectionMethod('SplStack', 'count'))->getClosure(new SplStack);
$c = $c->bindTo(new cls);
var_dump($c);

$c = (new ReflectionMethod('SplStack', 'count'))->getClosure(new SplStack);
$c = $c->bindTo(new SplStack, 'cls');
var_dump($c);

?>
--EXPECTF--
Warning: Cannot bind method SplDoublyLinkedList::count() to object of class cls, this will be an error in PHP 9 in %s on line %d
NULL

Warning: Cannot rebind scope of closure created from method, this will be an error in PHP 9 in %s on line %d
NULL
