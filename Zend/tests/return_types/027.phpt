--TEST--
Return type of parent is allowed in closure
--FILE--
<?php

class A {}
class B extends A {}

$c = function(parent $x): parent { return $x; };
var_dump($c->bindTo(null, 'B')(new A));

--EXPECTF--
object(A)#%d (0) {
}
