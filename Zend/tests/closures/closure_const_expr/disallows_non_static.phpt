--TEST--
Disallows using non-static closures.
--FILE--
<?php

class C {
    public Closure $d = function () {
        var_dump($this);
    };
}

$foo = new C();
var_dump($foo->d);
($foo->d)();

?>
--EXPECTF--
Fatal error: Closures in constant expressions must be static in %s on line %d
