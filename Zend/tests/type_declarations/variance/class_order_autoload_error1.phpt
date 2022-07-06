--TEST--
Variance error in the presence of autoloading (1)
--FILE--
<?php

spl_autoload_register(function($class) {
    if ($class === 'A') {
        class A {
            public function method() : C {}
        }
    } else if ($class == 'B') {
        class B extends A {
            public function method() : B {}
        }
    } else {
        class C extends B {
        }
    }
});

$b = new B;

?>
--EXPECTF--
Fatal error: Declaration of B::method(): B must be compatible with A::method(): C in %s on line %d
