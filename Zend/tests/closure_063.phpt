--TEST--
Force failure with Closure binding to unknown scopes/$this with Closure::bind()
--FILE--
<?php

function foo() { print "FAIL\n"; }
$x = (new ReflectionFunction("foo"))->getClosure();
$x->bindTo(new stdClass);

class a { function foo() { echo get_class($this), "\n"; } }
$x = (new ReflectionMethod("a", "foo"))->getClosure(new a);
$x->bindTo(new stdClass);

class c extends stdClass { function foo() { echo get_class($this), "\n"; } }
$x = (new ReflectionMethod("c", "foo"))->getClosure(new c);
$x->bindTo(new stdClass);

class b extends a {}
$x = (new ReflectionMethod("a", "foo"))->getClosure(new a);
$x->bindTo(new b)();
$x->bindTo(new b, "a")();
$x->bindTo(new b, "c");

$x = (new ReflectionMethod("a", "foo"))->getClosure(new a);
$x->bindTo(new a)();

class d { function foo() { print "Success\n"; yield; } }
class e extends d {}
$x = (new ReflectionMethod("d", "foo"))->getClosure(new d);
$x->bindTo(new d)()->current();
$x->bindTo(new e)()->current();
$x->bindTo(new e, "d")()->current();
$x->bindTo(new e, "e");

?>
--EXPECTF--

Warning: Cannot bind function foo to an object or class in %s on line %d

Warning: Cannot bind function a::foo to object of class stdClass in %s on line %d

Warning: Cannot bind function c::foo to object of class stdClass in %s on line %d
b
b

Warning: Cannot bind function a::foo to scope class c in %s on line %d
a
Success
Success
Success

Warning: Cannot bind function d::foo to scope class e in %s on line %d

