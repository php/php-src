--TEST--
Force failure with Closure binding to unknown scopes/$this with Closure::call()
--FILE--
<?php

function foo() { print "FAIL\n"; }
$x = (new ReflectionFunction("foo"))->getClosure();
$x->call(new stdClass);

class a { function foo() { echo get_class($this), "\n"; } }
$x = (new ReflectionMethod("a", "foo"))->getClosure(new a);
$x->call(new stdClass);

class b extends a {}
$x = (new ReflectionMethod("a", "foo"))->getClosure(new a);
$x->call(new b);

class c extends stdClass { function foo() { echo get_class($this), "\n"; } }
$x = (new ReflectionMethod("c", "foo"))->getClosure(new c);
$x->call(new stdClass);

$x = (new ReflectionMethod("a", "foo"))->getClosure(new a);
$x->call(new a);

class d { function foo() { print "Success\n"; yield; } }
$x = (new ReflectionMethod("d", "foo"))->getClosure(new d);
$x->call(new d)->current();

?>
--EXPECTF--

Warning: Cannot bind function foo to an object in %s on line %d

Warning: Cannot bind function a::foo to object of class stdClass in %s on line %d
b

Warning: Cannot bind function c::foo to object of class stdClass in %s on line %d
a
Success
