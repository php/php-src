--TEST--
Bug #78647: Outstanding dependency obligation
--FILE--
<?php

spl_autoload_register(function ($class) {
    if ($class == 'A') {
        class A {
            function m(): B {}
        }
    } elseif ($class == 'B') {
        class B extends A {
            function m(): X {}
        }
    } else {
        class C extends B {}
    }
});

new B;

?>
--EXPECTF--
Fatal error: Could not check compatibility between B::m(): X and A::m(): B, because class X is not available in %s on line %d
