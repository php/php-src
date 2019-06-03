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
Fatal error: Could not check compatibility between B::method(): C and A::method(): B, because class C is not available in %s on line %d
