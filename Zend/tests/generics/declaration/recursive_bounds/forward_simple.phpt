--TEST--
Recursive bounds: simple forward reference in bound
--FILE--
<?php
class Foo {}
function f<U: T, T: Foo>(U $x): T { return $x; }
var_dump(f(new Foo()));
?>
--EXPECTF--
object(Foo)#%d (0) {
}
