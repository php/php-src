--TEST--
Test Closure binding to unknown scopes with Closure::call()
--FILE--
<?php

function foo() { echo get_class($this), "\n"; }
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

// internal functions with unknown scope must fail
$x = (new ReflectionMethod("Closure", "bindTo"))->getClosure(function() {});
$x->call(new a);

?>
--EXPECTF--
stdClass
stdClass
b
stdClass
a
Success

Warning: Cannot bind closure of internal method Closure::bindTo to unrelated object of class a in %s on line %d
