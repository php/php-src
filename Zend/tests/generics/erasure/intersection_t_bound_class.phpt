--TEST--
Erasure: T in intersection position with class-like bound erases cleanly
--FILE--
<?php
interface Bar {}
interface Foo {}

function pickIface<T : Bar>(T & Foo $x): T & Foo { return $x; }

$r = new ReflectionFunction('pickIface');
echo "param: ", $r->getParameters()[0]->getType(), "\n";
echo "return: ", $r->getReturnType(), "\n";

class BarFoo implements Bar, Foo {}
var_dump(pickIface(new BarFoo) instanceof BarFoo);
?>
--EXPECT--
param: Bar&Foo
return: Bar&Foo
bool(true)
