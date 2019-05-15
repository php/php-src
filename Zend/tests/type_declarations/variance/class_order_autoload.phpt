--TEST--
Returns are covariant, but we don't allow the code due to class ordering (autoload variation)
--FILE--
<?php

spl_autoload_register(function($class) {
    if ($class === 'A') {
        class A {
            public function method() : B {}
        }
    } else if ($class == 'B') {
        class B extends A {
            public function method() : C {}
        }
    } else {
        class C extends B {
        }
    }
});

$c = new C;

?>
--EXPECTF--
Fatal error: Declaration of B::method(): C must be compatible with A::method(): B in %s on line %d
