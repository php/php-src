--TEST--
Duplicate named param in new in const expr does not leak the argument
--FILE--
<?php

class D {}
class C {
    public function __construct(public $a) {}
}

function f($x = new C(a: new D, a: new D)) {}

try {
    f();
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Named parameter $a overwrites previous argument
