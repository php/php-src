--TEST--
Closure 042: Binding an instance to a non-scoped non-static closures gives it a dummy scope
--SKIPIF--
<?php if(!extension_loaded("reflection")) print "skip no reflection"; ?>
--FILE--
<?php

$c = function() { var_dump($this); };
$d = $c->bindTo(new stdClass);
$d();
$rm = new ReflectionFunction($d);
var_dump($rm->getClosureScopeClass()->name); //dummy sope is Closure

//should have the same effect
$d = $c->bindTo(new stdClass, NULL);
$d();
$rm = new ReflectionFunction($d);
var_dump($rm->getClosureScopeClass()->name); //dummy sope is Closure

echo "Done.\n";

--EXPECTF--
object(stdClass)#%d (0) {
}
string(7) "Closure"
object(stdClass)#%d (0) {
}
string(7) "Closure"
Done.
